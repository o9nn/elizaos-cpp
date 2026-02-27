#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_REGISTRY_SCHEMA_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_REGISTRY_SCHEMA_H
#include "core.hpp"
#include "zod.hpp"

typedef any PluginType;

typedef z::infer<registrySchema> Registry;


extern any registrySchema;
template <typename RET>
RET getPluginType(string name);

template <typename RET>
RET getPluginType(string name)
{
    if ((new RegExp(std:("sq")))->test(name)) return std:("adapter");
    if ((new RegExp(std:("discord|twitter|telegra")))->test(name)) return std:("client");
    return std:("plugin");
};


#endif
