#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_EXPORT_TYPE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_DISCRUB_EXT_SRC_ENUM_EXPORT_TYPE_H
#include "core.hpp"
#include <string>

enum struct ExportType;

enum struct ExportType {
    JSON = std::string("json"), HTML = std::string("html"), CSV = std::string("csv"), MEDIA = std::string("media")
};
#endif
