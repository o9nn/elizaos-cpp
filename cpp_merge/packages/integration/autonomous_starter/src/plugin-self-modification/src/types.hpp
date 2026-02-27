#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-SELF-MODIFICATION_SRC_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-SELF-MODIFICATION_SRC_TYPES_H
#include "core.h"
#include "@elizaos/core.h"

enum struct ModelClass;

enum struct ModelClass {
    SMALL = std::string("small"), MEDIUM = std::string("medium"), LARGE = std::string("large"), EMBEDDING = std::string("embedding"), IMAGE = std::string("image")
};
#endif
