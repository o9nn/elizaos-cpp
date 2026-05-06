#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_AGENT_HOOKS_ABSTRACT_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_AGENT_HOOKS_ABSTRACT_H
#include "core.hpp"
#include "../../types.h"

class DefaultAgent;
class AbstractAgentHook;
class CombinedAgentHook;

class DefaultAgent : public object, public std::enable_shared_from_this<DefaultAgent> {
public:
    using std::enable_shared_from_this<DefaultAgent>::shared_from_this;
    string name;
};

class AbstractAgentHook : public object, public std::enable_shared_from_this<AbstractAgentHook> {
public:
    using std::enable_shared_from_this<AbstractAgentHook>::shared_from_this;
    virtual void onInit(std::shared_ptr<DefaultAgent> _agent);
    virtual void onRunStart();
    virtual void onStepStart();
    virtual void onActionsGenerated(std::shared_ptr<StepOutput> _step);
    virtual void onActionStarted(std::shared_ptr<StepOutput> _step);
    virtual void onActionExecuted(std::shared_ptr<StepOutput> _step);
    virtual void onStepDone(std::shared_ptr<StepOutput> _step, std::shared_ptr<AgentInfo> _info);
    virtual void onRunDone(array _trajectory, std::shared_ptr<AgentInfo> _info);
    virtual void onSetupAttempt();
    virtual void onModelQuery(array<object> _messages, string _agent);
    virtual void onQueryMessageAdded(object _params);
    virtual void onSetupDone();
    virtual void onToolsInstallationStarted();
};

class CombinedAgentHook : public AbstractAgentHook, public std::enable_shared_from_this<CombinedAgentHook> {
public:
    using std::enable_shared_from_this<CombinedAgentHook>::shared_from_this;
    array<std::shared_ptr<AbstractAgentHook>> hooks = array<std::shared_ptr<AbstractAgentHook>>();

    CombinedAgentHook(array<std::shared_ptr<AbstractAgentHook>> hooks = std::nullopt);
    virtual void addHook(std::shared_ptr<AbstractAgentHook> hook);
    virtual array<std::shared_ptr<AbstractAgentHook>> get_allHooks();
    virtual void onInit(std::shared_ptr<DefaultAgent> _agent);
    virtual void onRunStart();
    virtual void onStepStart();
    virtual void onActionsGenerated(std::shared_ptr<StepOutput> _step);
    virtual void onActionStarted(std::shared_ptr<StepOutput> _step);
    virtual void onActionExecuted(std::shared_ptr<StepOutput> _step);
    virtual void onStepDone(std::shared_ptr<StepOutput> _step, std::shared_ptr<AgentInfo> _info);
    virtual void onRunDone(array _trajectory, std::shared_ptr<AgentInfo> _info);
    virtual void onSetupAttempt();
    virtual void onModelQuery(array<object> _messages, string _agent);
    virtual void onQueryMessageAdded(object _params);
    virtual void onSetupDone();
    virtual void onToolsInstallationStarted();
};

#endif
