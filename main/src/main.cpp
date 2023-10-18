#include <iostream>
#include <memory>
#include <vector>
#include <filesystem>
#include <string.h>
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#include <libloaderapi.h>
#else
#include <dlfcn.h>
#endif

#include "PluginInterface.hpp"

class ProjectImplementation : public YourProjectInterface
{
public:
    void projectCallbackFunction() override
    {
        std::cout << "we got called!" << std::endl;
    }
};

class DlRaiiWrapper final
{
public:
    void* handle = nullptr;

    void* getHandle()
    {
        return handle;
    }

    void* getSymbol(const char* sym)
    {
        if (!handle)
        {
            return nullptr;
        }

#ifdef _WIN32
        return GetProcAddress(reinterpret_cast<HMODULE>(handle), sym);
#else
        return dlsym(handle, sym);
#endif
    }

    explicit DlRaiiWrapper(const char* file)
    {
#ifdef _WIN32
        handle = LoadLibrary(file);
#else
        handle = dlopen(file, RTLD_LAZY);
#endif
    }

    ~DlRaiiWrapper()
    {
        if (handle)
        {

#ifdef _WIN32
            FreeLibrary(reinterpret_cast<HMODULE>(handle));
#else
            dlclose(handle);
#endif
        }
    }
};

struct PluginInfo_s
{
    std::string name;
    std::unique_ptr<DlRaiiWrapper> handle;
    std::unique_ptr<YourPluginInterface> impl;
};

std::vector<PluginInfo_s> plugins{};

int main(int argc, char* argv[])
{
    assert(argc > 0);

    auto impl = std::make_unique<ProjectImplementation>();

    try
    {
        auto itr = std::filesystem::directory_iterator(std::filesystem::path(argv[0]).parent_path() / "plugins");

        for (const auto& f : itr)
        {
#ifdef _WIN32
            if (f.path().extension().string() != ".dll")
#else
            if (f.path().extension().string() != ".so")
#endif
            {
                continue;
            }

            auto pluginHandle = std::make_unique<DlRaiiWrapper>(f.path().string().c_str());

            if (!pluginHandle->getHandle())
            {
                std::cerr << "failed to load plugin " << f.path() << std::endl;
                continue;
            }

            auto* initializePluginSym = reinterpret_cast<YourPluginInterface* (*)(YourProjectInterface*)>(pluginHandle->getSymbol("initialize"));
            if (!initializePluginSym)
            {
                std::cerr << "failed to load plugin initialize symbol " << f.path() << std::endl;
                continue;
            }

            std::cout << "loading plugin " << f.path() << std::endl;

            auto plugin = std::unique_ptr<YourPluginInterface>(initializePluginSym(impl.get()));

            if (!plugin.get())
            {
                std::cerr << "not loading plugin" << std::endl;
                continue;
            }

            if (strncmp(plugin->magic, MAGICPLUGIN, sizeof(MAGICPLUGIN) - 1))
            {
                std::cerr << "plugin has bad magic" << std::endl;
                continue;
            }

            std::cout << "loaded plugin " << plugin->name << " version " << plugin->version.major << '.' << plugin->version.minor << '.' << plugin->version.patch << " by " << plugin->author << std::endl;
            plugins.push_back({ f.path().filename().string(), std::move(pluginHandle), std::move(plugin) });
        }
    }
    catch (std::filesystem::filesystem_error e)
    {
        std::cerr << e.what() << std::endl;
    }

    std::cout << "loaded " << plugins.size() << " plugins" << std::endl;

    for (const auto& plugin : plugins)
    {
        plugin.impl->pluginCallbackFunction();
    }

    plugins.clear();
    return 0;
}
