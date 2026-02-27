#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_TESTS_TEST_SUITES_DEVRELTESTSUITE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_TESTS_TEST_SUITES_DEVRELTESTSUITE_H
#include "core.hpp"
// External dependency removed
#include "uuid.hpp"
using uuidv4 = v4;

class DevRelTestSuite;

class DevRelTestSuite : public TestSuite, public std::enable_shared_from_this<DevRelTestSuite> {
public:
    using std::enable_shared_from_this<DevRelTestSuite>::shared_from_this;
    string name = std:("devrel");

    string description = std:("Developer Relations Agent Test Suite");

    any scenarioService;

    double completionTimeout;

    DevRelTestSuite(double completionTimeout = 10000);
    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("Test Documentation Query")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std:("Scenario service missing")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Doc Test"), std:("Test Developer")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("support")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("How do I implement custom actions in ElizaOS?")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std:("Documentation response timeout")));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Test Plugin Integration")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std:("Scenario service missing")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Plugin Test"), std:("Test Developer")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("integration")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("Can you help me integrate the Discord plugin?")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std:("Plugin integration timeout")));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Test Source Code Knowledge")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std:("Scenario service missing")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Code Test"), std:("Test Developer")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("code-review")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("Where is the AgentRuntime class implemented?")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std:("Source code reference timeout")));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Test Missing Documentation")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std:("Scenario service missing")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Doc Test"), std:("Test Developer")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("support")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("How do I implement custom actions in ElizaOS?")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std:("Documentation not found")));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Test Code Navigation")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std:("Scenario service missing")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Code Test"), std:("Test Developer")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("code-review")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("Where is the AgentRuntime class implemented?")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std:("Could not locate code reference")));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Test Documentation Reference")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std:("Scenario service missing")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Doc Test"), std:("Test Developer")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("support")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("How do I implement custom actions in ElizaOS?")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std:("Documentation reference missing")));
        }
        }
    } };
};

#endif
