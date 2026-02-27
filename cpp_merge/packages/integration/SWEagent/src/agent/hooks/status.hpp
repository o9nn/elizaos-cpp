#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_AGENT_HOOKS_STATUS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_AGENT_HOOKS_STATUS_H
#include "core.hpp"
#include "./abstract.h"
#include "../../types.h"

class SetStatusAgentHook;

class SetStatusAgentHook : public AbstractAgentHook, public std::enable_shared_from_this<SetStatusAgentHook> {
public:
    using std::enable_shared_from_this<SetStatusAgentHook>::shared_from_this;
    std::function<void(string, string)> callable;

    string id;

    double iStep = 0;

    double cost = 0;

    double iAttempt = 0;

    double previousCost = 0;

    SetStatusAgentHook(string id, std::function<void(string, string)> callable);
    virtual void onSetupAttempt();
    virtual void update(string message);
    virtual void onStepStart();
    virtual void onStepDone(std::shared_ptr<StepOutput> _step, std::shared_ptr<AgentInfo> info);
    virtual void onToolsInstallationStarted();
};

#endif
