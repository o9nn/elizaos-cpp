#include "abstract.hpp"

void AbstractAgentHook::onInit(std::shared_ptr<DefaultAgent> _agent)
{
}

void AbstractAgentHook::onRunStart()
{
}

void AbstractAgentHook::onStepStart()
{
}

void AbstractAgentHook::onActionsGenerated(std::shared_ptr<StepOutput> _step)
{
}

void AbstractAgentHook::onActionStarted(std::shared_ptr<StepOutput> _step)
{
}

void AbstractAgentHook::onActionExecuted(std::shared_ptr<StepOutput> _step)
{
}

void AbstractAgentHook::onStepDone(std::shared_ptr<StepOutput> _step, std::shared_ptr<AgentInfo> _info)
{
}

void AbstractAgentHook::onRunDone(array _trajectory, std::shared_ptr<AgentInfo> _info)
{
}

void AbstractAgentHook::onSetupAttempt()
{
}

void AbstractAgentHook::onModelQuery(array<object> _messages, string _agent)
{
}

void AbstractAgentHook::onQueryMessageAdded(object _params)
{
}

void AbstractAgentHook::onSetupDone()
{
}

void AbstractAgentHook::onToolsInstallationStarted()
{
}

CombinedAgentHook::CombinedAgentHook(array<std::shared_ptr<AbstractAgentHook>> hooks) : AbstractAgentHook() {
    this->hooks = OR((hooks), (array<any>()));
}

void CombinedAgentHook::addHook(std::shared_ptr<AbstractAgentHook> hook)
{
    this->hooks->push(hook);
}

array<std::shared_ptr<AbstractAgentHook>> CombinedAgentHook::get_allHooks()
{
    return this->hooks;
}

void CombinedAgentHook::onInit(std::shared_ptr<DefaultAgent> _agent)
{
    for (auto& hook : this->hooks)
    {
        hook->onInit(_agent);
    }
}

void CombinedAgentHook::onRunStart()
{
    for (auto& hook : this->hooks)
    {
        hook->onRunStart();
    }
}

void CombinedAgentHook::onStepStart()
{
    for (auto& hook : this->hooks)
    {
        hook->onStepStart();
    }
}

void CombinedAgentHook::onActionsGenerated(std::shared_ptr<StepOutput> _step)
{
    for (auto& hook : this->hooks)
    {
        hook->onActionsGenerated(_step);
    }
}

void CombinedAgentHook::onActionStarted(std::shared_ptr<StepOutput> _step)
{
    for (auto& hook : this->hooks)
    {
        hook->onActionStarted(_step);
    }
}

void CombinedAgentHook::onActionExecuted(std::shared_ptr<StepOutput> _step)
{
    for (auto& hook : this->hooks)
    {
        hook->onActionExecuted(_step);
    }
}

void CombinedAgentHook::onStepDone(std::shared_ptr<StepOutput> _step, std::shared_ptr<AgentInfo> _info)
{
    for (auto& hook : this->hooks)
    {
        hook->onStepDone(_step, _info);
    }
}

void CombinedAgentHook::onRunDone(array _trajectory, std::shared_ptr<AgentInfo> _info)
{
    for (auto& hook : this->hooks)
    {
        hook->onRunDone(_trajectory, _info);
    }
}

void CombinedAgentHook::onSetupAttempt()
{
    for (auto& hook : this->hooks)
    {
        hook->onSetupAttempt();
    }
}

void CombinedAgentHook::onModelQuery(array<object> _messages, string _agent)
{
    for (auto& hook : this->hooks)
    {
        hook->onModelQuery(_messages, _agent);
    }
}

void CombinedAgentHook::onQueryMessageAdded(object _params)
{
    for (auto& hook : this->hooks)
    {
        hook->onQueryMessageAdded(_params);
    }
}

void CombinedAgentHook::onSetupDone()
{
    for (auto& hook : this->hooks)
    {
        hook->onSetupDone();
    }
}

void CombinedAgentHook::onToolsInstallationStarted()
{
    for (auto& hook : this->hooks)
    {
        hook->onToolsInstallationStarted();
    }
}

