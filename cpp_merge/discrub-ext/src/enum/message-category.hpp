#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_MESSAGE_CATEGORY_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_MESSAGE_CATEGORY_H
#include "core.hpp"

enum struct MessageCategory;

enum struct MessageCategory {
    PINNED = std:("pinned"), REACTIONS = std:("reactions"), THREAD = std:("thread"), THREAD_STARTER = std:("threadStarter")
};
#endif
