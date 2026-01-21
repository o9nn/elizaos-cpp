#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_ENVIRONMENT_HOOKS_STATUS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_ENVIRONMENT_HOOKS_STATUS_H
#include "core.h"
#include "./abstract.h"
#include "../repo.h"

typedef std::function<void(std::string, std::string)> StatusCallback;

class SetStatusEnvironmentHook;

class SetStatusEnvironmentHook : public EnvHook, public std::enable_shared_from_this<SetStatusEnvironmentHook> {
public:
    using std::enable_shared_from_this<SetStatusEnvironmentHook>::shared_from_this;
    StatusCallback callable;

    std::string id;

    SetStatusEnvironmentHook(std::string id, StatusCallback callable);
    virtual void update(std::string message);
    template <typename P0>
    void onCopyRepoStarted(P0 repo);
    virtual void onStartDeployment();
    virtual void onInstallEnvStarted();
    virtual void onEnvironmentStartup();
    virtual void onClose();
};

template <typename P0>
void SetStatusEnvironmentHook::onCopyRepoStarted(P0 repo)
{
    auto repoName = (in(std::string("repoName"), repo)) ? std::any(repo["repoName"]) : std::any(std::string("unknown"));
    this->update(std::string("Copying repo ") + repoName + string_empty);
}

#endif
