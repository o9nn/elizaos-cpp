#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_ENUM_MESSAGE-CATEGORY_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_ENUM_MESSAGE-CATEGORY_H
#include "core.h"

enum struct MessageCategory;

enum struct MessageCategory {
    PINNED = std::string("pinned"), REACTIONS = std::string("reactions"), THREAD = std::string("thread"), THREAD_STARTER = std::string("threadStarter")
};
#endif
