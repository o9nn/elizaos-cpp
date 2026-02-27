#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_EMBED_TYPE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_EMBED_TYPE_H
#include "core.hpp"
#include <string>

enum struct EmbedType;

enum struct EmbedType {
    RICH = std::string("rich"), IMAGE = std::string("image"), VIDEO = std::string("video"), GIFV = std::string("gifv"), ARTICLE = std::string("article"), LINK = std::string("link")
};
#endif
