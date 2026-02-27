#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PLUGIN_BOOTSTRAP_SRC___TESTS___SERVICES_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PLUGIN_BOOTSTRAP_SRC___TESTS___SERVICES_TEST_H
#include "core.hpp"
#include "bun:test.h"
#include "../services/task.h"
// External dependency removed
#include "../index.h"
// External dependency removed
#include "./test-utils.h"

class PluginService;

class PluginService : public Service, public std::enable_shared_from_this<PluginService> {
public:
    using std::enable_shared_from_this<PluginService>::shared_from_this;
    string type;

    string name;

    std::function<std::shared_ptr<Promise<any>>(std::shared_ptr<IAgentRuntime>)> init;
};

extern std::function<array<std::shared_ptr<PluginService>>()> getPluginServices;
#endif
