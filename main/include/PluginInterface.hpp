#pragma once

#define MAGICPLUGIN "GULP"
#define MAGICPROJECT "PLUG"

#define INTERFACE_VERSION_MAJOR 0
#define INTERFACE_VERSION_MINOR 0
#define INTERFACE_VERSION_PATCH 1

struct version_s
{
    int major = 0;
    int minor = 0;
    int patch = 0;
};

class YourProjectInterface
{
public:
    char magic[4] = {'P', 'L', 'U', 'G'};
    version_s version = { INTERFACE_VERSION_MAJOR, INTERFACE_VERSION_MINOR, INTERFACE_VERSION_PATCH };

    virtual void projectCallbackFunction() = 0;
};

class YourPluginInterface
{
public:
    char magic[4] = {'G', 'U', 'L', 'P'};
    char name[64];
    char author[64];
    version_s version;

    virtual void pluginCallbackFunction() = 0;
};
