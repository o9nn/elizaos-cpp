#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_FILTER_TYPE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_FILTER_TYPE_H
#include "core.hpp"
#include <string>

enum struct FilterType;

enum struct FilterType {
    TEXT = std::string("text"), DATE = std::string("date"), THREAD = std::string("thread"), TOGGLE = std::string("toggle"), ARRAY = std::string("array")
};
#endif
