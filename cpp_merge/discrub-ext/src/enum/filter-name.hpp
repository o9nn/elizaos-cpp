#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_FILTER_NAME_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_FILTER_NAME_H
#include "core.hpp"
#include <string>

enum struct FilterName;

enum struct FilterName {
    INVERSE = std::string("inverse"), ATTACHMENT_NAME = std::string("attachmentName"), CONTENT = std::string("content"), START_TIME = std::string("startTime"), END_TIME = std::string("endTime"), USER_NAME = std::string("userName"), MESSAGE_TYPE = std::string("messageType")
};
#endif
