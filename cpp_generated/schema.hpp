#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_REGISTRY_SCHEMA_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_UTILS_REGISTRY_SCHEMA_H
#include "core.h"
#include "zod.h"

typedef any PluginType;

typedef z::infer<registrySchema> Registry;


extern any registrySchema;
template <typename RET>
RET getPluginType(string name);

template <typename RET>
RET getPluginType(string name)
{
    if ((new RegExp(std::string("sq")))->test(name)) return std::string("adapter");
    if ((new RegExp(std::string("discord|twitter|telegra")))->test(name)) return std::string("client");
    return std::string("plugin");
};


#endif
