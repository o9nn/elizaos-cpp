#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_ENVIRONMENT_HOOKS_ABSTRACT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_ENVIRONMENT_HOOKS_ABSTRACT_H
#include "core.h"
#include "../repo.h"

class EnvironmentInstance;
class EnvHook;
class CombinedEnvHooks;

class EnvironmentInstance : public object, public std::enable_shared_from_this<EnvironmentInstance> {
public:
    using std::enable_shared_from_this<EnvironmentInstance>::shared_from_this;
    any deployment;

    any repo;
};

class EnvHook : public object, public std::enable_shared_from_this<EnvHook> {
public:
    using std::enable_shared_from_this<EnvHook>::shared_from_this;
    virtual void onInit(std::shared_ptr<EnvironmentInstance> _env);
    template <typename P0>
    void onCopyRepoStarted(P0 _repo);
    virtual void onStartDeployment();
    virtual void onInstallEnvStarted();
    virtual void onClose();
    virtual void onEnvironmentStartup();
};

class CombinedEnvHooks : public EnvHook, public std::enable_shared_from_this<CombinedEnvHooks> {
public:
    using std::enable_shared_from_this<CombinedEnvHooks>::shared_from_this;
    array<std::shared_ptr<EnvHook>> hooks = array<std::shared_ptr<EnvHook>>();

    virtual void addHook(std::shared_ptr<EnvHook> hook);
    virtual void onInit(std::shared_ptr<EnvironmentInstance> env);
    template <typename P0>
    void onCopyRepoStarted(P0 repo);
    virtual void onStartDeployment();
    virtual void onInstallEnvStarted();
    virtual void onClose();
    virtual void onEnvironmentStartup();
};

template <typename P0>
void EnvHook::onCopyRepoStarted(P0 _repo)
{
}

template <typename P0>
void CombinedEnvHooks::onCopyRepoStarted(P0 repo)
{
    for (auto& hook : this->hooks)
    {
        try
        {
            hook->onCopyRepoStarted(repo);
        }
        catch (const any& error)
        {
            console->error(std::string("Hook error in onCopyRepoStarted:"), error);
        }
    }
}

#endif
