#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-ELIZAOS-SERVICES_SRC_AUTH_CLIAUTHCOMMANDS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-ELIZAOS-SERVICES_SRC_AUTH_CLIAUTHCOMMANDS_H
#include "core.h"
#include "@elizaos/core.h"
#include "./AuthenticationService.js.h"

class CLICommand;
class CLIOption;
class CLIAuthCommands;

class CLICommand : public object, public std::enable_shared_from_this<CLICommand> {
public:
    using std::enable_shared_from_this<CLICommand>::shared_from_this;
    string name;

    string description;

    array<std::shared_ptr<CLIOption>> options;

    std::function<std::shared_ptr<Promise<void>>(any, std::shared_ptr<IAgentRuntime>)> handler;
};

class CLIOption : public object, public std::enable_shared_from_this<CLIOption> {
public:
    using std::enable_shared_from_this<CLIOption>::shared_from_this;
    string name;

    string description;

    boolean required;

    any type;
};

class CLIAuthCommands : public object, public std::enable_shared_from_this<CLIAuthCommands> {
public:
    using std::enable_shared_from_this<CLIAuthCommands>::shared_from_this;
    std::shared_ptr<AuthenticationService> authService;

    CLIAuthCommands(std::shared_ptr<IAgentRuntime> runtime);
    virtual array<std::shared_ptr<CLICommand>> getCommands();
    virtual std::shared_ptr<Promise<void>> handleAuthStatus();
    virtual std::shared_ptr<Promise<void>> handleAuthTest();
    virtual std::shared_ptr<Promise<void>> handleAuthValidate(object args);
    virtual std::shared_ptr<Promise<void>> handleTestKeys();
    virtual std::shared_ptr<Promise<void>> handleClearCache();
    virtual std::shared_ptr<Promise<void>> handleSetup();
    virtual string getStatusIcon(string status);
};

array<std::shared_ptr<CLICommand>> registerAuthCommands(std::shared_ptr<IAgentRuntime> runtime);

#endif
