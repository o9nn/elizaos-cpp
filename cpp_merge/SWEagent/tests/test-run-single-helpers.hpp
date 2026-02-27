#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_TESTS_TEST-RUN-SINGLE-HELPERS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_TESTS_TEST-RUN-SINGLE-HELPERS_H
#include "core.h"
#include "../src/environment/deployment.h"

class MockRuntime;
class MockDeployment;

class MockRuntime : public AbstractRuntime, public std::enable_shared_from_this<MockRuntime> {
public:
    using std::enable_shared_from_this<MockRuntime>::shared_from_this;
    std::shared_ptr<Map<string, string>> files = std::make_shared<Map>();

    string sessionOutput = string_empty;

    virtual std::shared_ptr<Promise<void>> createSession(std::shared_ptr<CreateBashSessionRequest> _request);
    template <typename P0>
    std::shared_ptr<Promise<std::shared_ptr<BashActionResult>>> runInSession(P0 action);
    virtual std::shared_ptr<Promise<std::shared_ptr<CommandResult>>> execute(std::shared_ptr<Command> _command);
    virtual std::shared_ptr<Promise<std::shared_ptr<ReadFileResponse>>> readFile(std::shared_ptr<ReadFileRequest> request);
    virtual std::shared_ptr<Promise<void>> writeFile(std::shared_ptr<WriteFileRequest> request);
    virtual std::shared_ptr<Promise<void>> upload(std::shared_ptr<UploadRequest> _request);
};

class MockDeployment : public AbstractDeployment, public std::enable_shared_from_this<MockDeployment> {
public:
    using std::enable_shared_from_this<MockDeployment>::shared_from_this;
    std::shared_ptr<MockRuntime> runtime;

    MockDeployment();
    virtual std::shared_ptr<Promise<void>> start();
    virtual std::shared_ptr<Promise<void>> stop();
};

template <typename P0>
std::shared_ptr<Promise<std::shared_ptr<BashActionResult>>> MockRuntime::runInSession(P0 action)
{
    if (AND((in(std:("type"), action)), (action->type == std:("interrupt")))) {
        return object{
            object::pair{std:("output"), string_empty}, 
            object::pair{std:("exitCode"), 0}
        };
    }
    auto bashAction = as<std::shared_ptr<BashAction>>(action);
    if (bashAction->command->startsWith(std:("echo "))) {
        auto text = bashAction->command->substring(5)->replace((new RegExp(std:("['"]"))), string_empty);
        return object{
            object::pair{std:("output"), text + std:("\
")}, 
            object::pair{std:("exitCode"), 0}
        };
    }
    if (bashAction->command == std:("ls")) {
        return object{
            object::pair{std:("output"), std:("file1\
file2\
")}, 
            object::pair{std:("exitCode"), 0}
        };
    }
    if (bashAction->command->startsWith(std:("sleep "))) {
        auto seconds = parseFloat(bashAction->command->substring(6));
        if (AND((bashAction->timeout), (bashAction->timeout < seconds))) {
            throw any(std::make_shared<Error>(std:("Command timeout")));
        }
        return object{
            object::pair{std:("output"), string_empty}, 
            object::pair{std:("exitCode"), 0}
        };
    }
    return object{
        object::pair{std:("output"), this->sessionOutput}, 
        object::pair{std:("exitCode"), 0}
    };
}

#endif
