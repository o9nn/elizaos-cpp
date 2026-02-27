#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_DYNAMIC_EXAMPLES_EXAMPLE_USAGE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_DYNAMIC_EXAMPLES_EXAMPLE_USAGE_H
#include "core.hpp"
// External dependency removed
#include "../index.h"

void createWeatherPlugin(std::shared_ptr<Agent> agent);

void createTodoPlugin(std::shared_ptr<Agent> agent);

void monitorPluginCreation(std::shared_ptr<Agent> agent);

any getDatabasePluginSpec();

any getSocialMediaPluginSpec();

void main();

#endif
