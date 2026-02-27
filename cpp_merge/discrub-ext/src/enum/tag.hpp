#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_TAG_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_TAG_H
#include "core.hpp"

enum struct Tag;

enum struct Tag {
    TAGS_MADE_BY_USER = 0, TAGGED_USERS = 1
};
extern std::function<string(Tag)> getTagName;
#endif
