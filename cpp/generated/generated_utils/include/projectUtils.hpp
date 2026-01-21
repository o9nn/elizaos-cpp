#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_UTILS_PROJECTUTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-LIST_SRC_UTILS_PROJECTUTILS_H
#include "core.h"
#include "@/types/project.h"

extern std::function<array<any>(array<any>, std::any)> sortProjects;
extern std::function<std::string()> generateProjectId;
extern std::function<array<string>(std::any)> validateProject;
#endif
