#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_SELF_MODIFICATION_SRC_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_SELF_MODIFICATION_SRC_TYPES_H
#include "core.hpp"
#include <string>
// External dependency removed

enum struct ModelClass;

enum struct ModelClass {
    SMALL = std::string("small"), MEDIUM = std::string("medium"), LARGE = std::string("large"), EMBEDDING = std::string("embedding"), IMAGE = std::string("image")
};
#endif
