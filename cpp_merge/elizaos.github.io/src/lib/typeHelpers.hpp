#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAOS_GITHUB_IO_SRC_LIB_TYPEHELPERS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAOS_GITHUB_IO_SRC_LIB_TYPEHELPERS_H
#include "core.hpp"

extern std::function<any(std::shared_ptr<T>)> isNotNullOrUndefined;
extern std::function<any(array<std::shared_ptr<T>>)> isNotNullOrUndefinedArray;
extern std::function<std::function<any(std::shared_ptr<T>)>(std::shared_ptr<K>)> isNotNullOrUndefinedField;
#endif
