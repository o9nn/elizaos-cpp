#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_HAS_TYPE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_HAS_TYPE_H
#include "core.hpp"
#include <string>

enum struct HasType;

enum struct HasType {
    EMBED = std::string("embed"), FILE = std::string("file"), IMAGE = std::string("image"), LINK = std::string("link"), SOUND = std::string("sound"), STICKER = std::string("sticker"), VIDEO = std::string("video")
};
#endif
