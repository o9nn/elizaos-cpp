#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RULES_PROJECT-OVERVIEW_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RULES_PROJECT-OVERVIEW_H
#include "core.h"
#include "./types.h"

extern array<std::shared_ptr<EntryPoint>> ENTRY_POINTS;
extern std::shared_ptr<ClassInfo> MAIN_AGENT_CLASS;
extern std::shared_ptr<ClassInfo> SWE_ENV_CLASS;
extern std::shared_ptr<ExecutionEnvironment> EXECUTION_ENVIRONMENT;
extern std::shared_ptr<ToolsInfo> TOOLS_INFO;
extern array<std::shared_ptr<Inspector>> INSPECTORS;
extern std::shared_ptr<ProjectStructure> PROJECT_STRUCTURE;
extern std::string PROJECT_OVERVIEW;
std::any getComponentByPath(std::string path);

array<string> getPythonModules();

extern Record<std::string, string> TYPESCRIPT_EQUIVALENTS;
#endif
