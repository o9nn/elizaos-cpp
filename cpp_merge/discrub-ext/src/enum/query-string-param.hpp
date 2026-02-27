#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_QUERY_STRING_PARAM_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_QUERY_STRING_PARAM_H
#include "core.hpp"
#include <string>

enum struct QueryStringParam;

enum struct QueryStringParam {
    AROUND = std::string("around"), BEFORE = std::string("before"), AFTER = std::string("after")
};
#endif
