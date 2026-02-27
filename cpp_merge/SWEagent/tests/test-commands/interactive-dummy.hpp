#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_TESTS_TEST_COMMANDS_INTERACTIVE_DUMMY_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_TESTS_TEST_COMMANDS_INTERACTIVE_DUMMY_H
#include "core.hpp"
#include "readline.hpp"

class InteractiveDummyCommand;

class InteractiveDummyCommand : public object, public std::enable_shared_from_this<InteractiveDummyCommand> {
public:
    using std::enable_shared_from_this<InteractiveDummyCommand>::shared_from_this;
    static string PROMPT;

    std::shared_ptr<readline::Interface> rl;

    InteractiveDummyCommand();
    virtual void start();
    virtual void send(string input);
    virtual void stop();
    virtual void run();
};

#endif
