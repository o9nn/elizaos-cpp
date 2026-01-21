#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-DYNAMIC_UTILS_PLUGIN-TEMPLATES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-DYNAMIC_UTILS_PLUGIN-TEMPLATES_H
#include "core.h"

extern std::function<std::string(std::string, std::string, std::shared_ptr<Record<std::string, any>>)> generateActionCode;
extern std::function<std::string(std::string, std::string, std::shared_ptr<Record<std::string, any>>)> generateProviderCode;
extern std::function<std::string(std::string, std::string, array<string>)> generateServiceCode;
extern std::function<std::string(std::string, std::string, array<string>)> generateEvaluatorCode;
extern std::function<std::string(std::string, std::any)> generatePluginIndex;
extern std::function<std::string(std::string, std::string)> generateTestCode;
extern std::function<std::string(std::string, std::string, std::shared_ptr<Record<std::string, any>>)> actionTemplate;
extern std::function<std::string(std::string, std::string, std::shared_ptr<Record<std::string, any>>)> providerTemplate;
extern std::function<std::string(std::string, std::string, array<string>)> serviceTemplate;
extern std::function<std::string(std::string, std::string, array<string>)> evaluatorTemplate;
extern std::function<std::string(std::string, std::any)> pluginIndexTemplate;
extern std::function<std::string(std::string, std::string)> testTemplate;
#endif
