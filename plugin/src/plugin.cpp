#include <iostream>
#include <memory>
#include <string.h>

#include "PluginInterface.hpp"

YourProjectInterface* impl_instance;

class PluginImplementation : public YourPluginInterface
{
public:
    void pluginCallbackFunction() override
    {
        std::cout << "HELLO WORK!!!!" << std::endl;

        impl_instance->projectCallbackFunction();
    }
};

PluginImplementation* plugin_impl_instance;

extern "C"
{
#if _MSC_VER
    __declspec(dllexport)
#endif
    YourPluginInterface* initialize(YourProjectInterface* impl)
    {
        if (strncmp(impl->magic, MAGICPROJECT, sizeof(MAGICPROJECT) - 1))
        {
            std::cerr << "project has bad magic" << std::endl;
            return nullptr;
        }

        if (impl->version.major != INTERFACE_VERSION_MAJOR)
        {
            std::cerr << "project major version mismatch" << std::endl;
            return nullptr;
        }

        if (impl->version.minor != INTERFACE_VERSION_MINOR)
        {
            std::cerr << "project minor version mismatch" << std::endl;
        }

        auto plug_impl = std::make_unique<PluginImplementation>();

        plug_impl->version.major = 0;
        plug_impl->version.minor = 0;
        plug_impl->version.patch = 1;

#if _MSC_VER
        strcpy_s(plug_impl->name, "plugin");
#else
        strncpy(plug_impl->name, "plugin", sizeof(plug_impl->name));
        plug_impl->name[sizeof(plug_impl->name) - 1] = '\0';
#endif

#if _MSC_VER
        strcpy_s(plug_impl->author, "me");
#else
        strncpy(plug_impl->author, "me", sizeof(plug_impl->author));
        plug_impl->author[sizeof(plug_impl->author) - 1] = '\0';
#endif

        impl_instance = impl;
        plugin_impl_instance = plug_impl.release();
        return plugin_impl_instance;
    }
}
