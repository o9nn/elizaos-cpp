#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_ENUM_FILTER-NAME_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_ENUM_FILTER-NAME_H
#include "core.h"

enum struct FilterName;

enum struct FilterName {
    INVERSE = std::string("inverse"), ATTACHMENT_NAME = std::string("attachmentName"), CONTENT = std::string("content"), START_TIME = std::string("startTime"), END_TIME = std::string("endTime"), USER_NAME = std::string("userName"), MESSAGE_TYPE = std::string("messageType")
};
#endif
