#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_ENUM_FILTER-TYPE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_ENUM_FILTER-TYPE_H
#include "core.h"

enum struct FilterType;

enum struct FilterType {
    TEXT = std::string("text"), DATE = std::string("date"), THREAD = std::string("thread"), TOGGLE = std::string("toggle"), ARRAY = std::string("array")
};
#endif
