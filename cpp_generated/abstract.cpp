#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/environment/hooks/abstract.h"

void EnvHook::onInit(std::shared_ptr<EnvironmentInstance> _env)
{
}

void EnvHook::onStartDeployment()
{
}

void EnvHook::onInstallEnvStarted()
{
}

void EnvHook::onClose()
{
}

void EnvHook::onEnvironmentStartup()
{
}

void CombinedEnvHooks::addHook(std::shared_ptr<EnvHook> hook)
{
    this->hooks->push(hook);
}

void CombinedEnvHooks::onInit(std::shared_ptr<EnvironmentInstance> env)
{
    for (auto& hook : this->hooks)
    {
        try
        {
            hook->onInit(env);
        }
        catch (const any& error)
        {
            console->error(std::string("Hook error in onInit:"), error);
        }
    }
}

void CombinedEnvHooks::onStartDeployment()
{
    for (auto& hook : this->hooks)
    {
        try
        {
            hook->onStartDeployment();
        }
        catch (const any& error)
        {
            console->error(std::string("Hook error in onStartDeployment:"), error);
        }
    }
}

void CombinedEnvHooks::onInstallEnvStarted()
{
    for (auto& hook : this->hooks)
    {
        try
        {
            hook->onInstallEnvStarted();
        }
        catch (const any& error)
        {
            console->error(std::string("Hook error in onInstallEnvStarted:"), error);
        }
    }
}

void CombinedEnvHooks::onClose()
{
    for (auto& hook : this->hooks)
    {
        try
        {
            hook->onClose();
        }
        catch (const any& error)
        {
            console->error(std::string("Hook error in onClose:"), error);
        }
    }
}

void CombinedEnvHooks::onEnvironmentStartup()
{
    for (auto& hook : this->hooks)
    {
        try
        {
            hook->onEnvironmentStartup();
        }
        catch (const any& error)
        {
            console->error(std::string("Hook error in onEnvironmentStartup:"), error);
        }
    }
}

