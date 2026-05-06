#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLIENT_SRC_CONSTANTS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLIENT_SRC_CONSTANTS_H
#include "core.hpp"
#include <string>

enum struct FIELD_REQUIREMENT_TYPE;

extern string USER_NAME;
extern string CHAT_SOURCE;
extern string GROUP_CHAT_SOURCE;
extern double AVATAR_IMAGE_MAX_SIZE;
enum struct FIELD_REQUIREMENT_TYPE {
    REQUIRED = std::string("required"), OPTIONAL = std::string("optional")
};
extern object FIELD_REQUIREMENTS;
#endif
