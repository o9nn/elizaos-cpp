#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_MESSAGE_CATEGORY_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_MESSAGE_CATEGORY_H
#include "core.hpp"
#include <string>

enum struct MessageCategory;

enum struct MessageCategory {
    PINNED = std::string("pinned"), REACTIONS = std::string("reactions"), THREAD = std::string("thread"), THREAD_STARTER = std::string("threadStarter")
};
#endif
