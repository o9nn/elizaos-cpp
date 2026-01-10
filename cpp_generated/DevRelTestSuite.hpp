#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_TESTS_TEST_SUITES_DEVRELTESTSUITE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_TESTS_TEST_SUITES_DEVRELTESTSUITE_H
#include "core.h"
#include "@elizaos/core.h"
#include "uuid.h"
using uuidv4 = v4;

class DevRelTestSuite;

class DevRelTestSuite : public TestSuite, public std::enable_shared_from_this<DevRelTestSuite> {
public:
    using std::enable_shared_from_this<DevRelTestSuite>::shared_from_this;
    string name = std::string("devrel");

    string description = std::string("Developer Relations Agent Test Suite");

    any scenarioService;

    double completionTimeout;

    DevRelTestSuite(double completionTimeout = 10000);
    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("Test Documentation Query")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std::string("Scenario service missing")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Doc Test"), std::string("Test Developer")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("support")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("How do I implement custom actions in ElizaOS?")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std::string("Documentation response timeout")));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Plugin Integration")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std::string("Scenario service missing")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Plugin Test"), std::string("Test Developer")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("integration")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("Can you help me integrate the Discord plugin?")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std::string("Plugin integration timeout")));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Source Code Knowledge")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std::string("Scenario service missing")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Code Test"), std::string("Test Developer")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("code-review")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("Where is the AgentRuntime class implemented?")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std::string("Source code reference timeout")));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Missing Documentation")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std::string("Scenario service missing")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Doc Test"), std::string("Test Developer")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("support")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("How do I implement custom actions in ElizaOS?")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std::string("Documentation not found")));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Code Navigation")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std::string("Scenario service missing")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Code Test"), std::string("Test Developer")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("code-review")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("Where is the AgentRuntime class implemented?")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std::string("Could not locate code reference")));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Documentation Reference")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std::string("Scenario service missing")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Doc Test"), std::string("Test Developer")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("support")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("How do I implement custom actions in ElizaOS?")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std::string("Documentation reference missing")));
        }
        }
    } };
};

#endif
