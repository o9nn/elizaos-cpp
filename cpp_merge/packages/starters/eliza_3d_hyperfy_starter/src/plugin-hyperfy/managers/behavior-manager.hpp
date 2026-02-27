#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-3D-HYPERFY-STARTER_SRC_PLUGIN-HYPERFY_MANAGERS_BEHAVIOR-MANAGER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-3D-HYPERFY-STARTER_SRC_PLUGIN-HYPERFY_MANAGERS_BEHAVIOR-MANAGER_H
#include "core.h"
#include "@elizaos/core.h"
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
