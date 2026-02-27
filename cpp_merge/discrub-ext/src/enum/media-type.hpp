#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_MEDIA_TYPE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_MEDIA_TYPE_H
#include "core.hpp"
#include <string>

enum struct MediaType;

enum struct MediaType {
    VIDEOS = std::string("videos"), IMAGES = std::string("images"), AUDIO = std::string("audio")
};
#endif
