#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_ENUM_EXPORT-TYPE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_DISCRUB-EXT_SRC_ENUM_EXPORT-TYPE_H
#include "core.h"

enum struct ExportType;

enum struct ExportType {
    JSON = std::string("json"), HTML = std::string("html"), CSV = std::string("csv"), MEDIA = std::string("media")
};
#endif
