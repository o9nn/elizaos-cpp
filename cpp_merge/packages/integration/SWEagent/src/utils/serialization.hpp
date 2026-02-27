#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_UTILS_SERIALIZATION_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_UTILS_SERIALIZATION_H
#include "core.hpp"
#include "js-yaml.h"
// Using alias removed (invalid transpilation)

any convertToYamlLiteralString(any data);

string yamlSerializationWithLinebreaks(any data);

any mergeNestedDicts(any d1, any d2);

#endif
