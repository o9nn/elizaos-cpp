#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-DYNAMIC_EXAMPLES_EXAMPLE-USAGE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-DYNAMIC_EXAMPLES_EXAMPLE-USAGE_H
#include "core.h"
#include "@elizaos/core.h"
#include "../index.h"

void createWeatherPlugin(std::shared_ptr<Agent> agent);

void createTodoPlugin(std::shared_ptr<Agent> agent);

void monitorPluginCreation(std::shared_ptr<Agent> agent);

any getDatabasePluginSpec();

any getSocialMediaPluginSpec();

void main();

#endif
