#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/run/hooks/types.h"

void AbstractRunHook::onInit(Record<string, any> _run)
{
}

void AbstractRunHook::onStart()
{
}

void AbstractRunHook::onEnd()
{
}

void AbstractRunHook::onInstanceStart(object _params)
{
}

void AbstractRunHook::onInstanceSkipped()
{
}

void AbstractRunHook::onInstanceCompleted(object _params)
{
}

void CombinedRunHooks::addHook(std::shared_ptr<RunHook> hook)
{
    this->_hooks->push(hook);
}

array<std::shared_ptr<RunHook>> CombinedRunHooks::get_hooks()
{
    return this->_hooks;
}

void CombinedRunHooks::onInit(Record<string, any> run)
{
    for (auto& hook : this->_hooks)
    {
        hook->onInit(run);
    }
}

void CombinedRunHooks::onStart()
{
    for (auto& hook : this->_hooks)
    {
        hook->onStart();
    }
}

void CombinedRunHooks::onEnd()
{
    for (auto& hook : this->_hooks)
    {
        hook->onEnd();
    }
}

void CombinedRunHooks::onInstanceStart(object params)
{
    for (auto& hook : this->_hooks)
    {
        hook->onInstanceStart(params);
    }
}

void CombinedRunHooks::onInstanceSkipped()
{
    for (auto& hook : this->_hooks)
    {
        hook->onInstanceSkipped();
    }
}

void CombinedRunHooks::onInstanceCompleted(object params)
{
    for (auto& hook : this->_hooks)
    {
        hook->onInstanceCompleted(params);
    }
}

