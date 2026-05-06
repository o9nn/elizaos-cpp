#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_RESOLUTION_TYPE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_RESOLUTION_TYPE_H
#include "core.hpp"
#include <string>

enum struct ResolutionType;

enum struct ResolutionType {
    HOVER_LIMITED = std::string("hoverLimited"), HOVER_FULL = std::string("hoverFull"), NO_HOVER_LIMITED = std::string("noHoverLimited"), NO_HOVER_FULL = std::string("noHoverFull")
};
#endif
