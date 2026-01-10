#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_DEV_UTILS_SERVER-MANAGER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_DEV_UTILS_SERVER-MANAGER_H
#include "core.h"
#include "node:child_process.h"
#include "node:child_process.h"
#include "../types.h"

class DevServerManager;

class DevServerManager : public ServerProcess, public std::enable_shared_from_this<DevServerManager> {
public:
    using std::enable_shared_from_this<DevServerManager>::shared_from_this;
    any process = nullptr;

    virtual std::shared_ptr<Promise<void>> stop();
    virtual std::shared_ptr<Promise<void>> start(array<string> args = array<string>());
    virtual std::shared_ptr<Promise<void>> restart(array<string> args = array<string>());
};

extern any serverInstance;
std::shared_ptr<DevServerManager> getServerManager();

std::shared_ptr<Promise<void>> stopServer();

std::shared_ptr<Promise<void>> startServer(array<string> args = array<string>());

std::shared_ptr<Promise<void>> restartServer(array<string> args = array<string>());

#endif
