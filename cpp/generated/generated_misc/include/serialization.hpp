#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_UTILS_SERIALIZATION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_UTILS_SERIALIZATION_H
#include "core.h"
#include "js-yaml.h"
using yaml = _default;

std::any convertToYamlLiteralString(std::any data);

std::string yamlSerializationWithLinebreaks(std::any data);

std::any mergeNestedDicts(std::any d1, std::any d2);

#endif
