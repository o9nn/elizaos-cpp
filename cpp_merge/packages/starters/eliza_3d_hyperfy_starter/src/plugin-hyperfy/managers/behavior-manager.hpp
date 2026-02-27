#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_3D_HYPERFY_STARTER_SRC_PLUGIN_HYPERFY_MANAGERS_BEHAVIOR_MANAGER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_3D_HYPERFY_STARTER_SRC_PLUGIN_HYPERFY_MANAGERS_BEHAVIOR_MANAGER_H
#include "core.hpp"
// External dependency removed
#include "../service.h"
#include "../templates.h"
#include "./guards.h"
#include "../utils.h"

class BehaviorManager;

extern double TIME_INTERVAL_MIN;
extern double TIME_INTERVAL_MAX;
class BehaviorManager : public object, public std::enable_shared_from_this<BehaviorManager> {
public:
    using std::enable_shared_from_this<BehaviorManager>::shared_from_this;
    boolean isRunning = false;

    std::shared_ptr<IAgentRuntime> runtime;

    BehaviorManager(std::shared_ptr<IAgentRuntime> runtime);
    virtual void start();
    virtual void stop();
    virtual std::shared_ptr<Promise<void>> runLoop();
    virtual any getService();
    virtual std::shared_ptr<Promise<void>> executeBehavior();
};

#endif
