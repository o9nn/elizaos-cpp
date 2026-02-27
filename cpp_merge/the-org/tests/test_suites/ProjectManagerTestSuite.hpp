#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_TESTS_TEST_SUITES_PROJECTMANAGERTESTSUITE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_TESTS_TEST_SUITES_PROJECTMANAGERTESTSUITE_H
#include "core.h"
#include "@elizaos/core.h"
#include "uuid.h"
using uuidv4 = v4;

class ProjectManagerTestSuite;

class ProjectManagerTestSuite : public TestSuite, public std::enable_shared_from_this<ProjectManagerTestSuite> {
public:
    using std::enable_shared_from_this<ProjectManagerTestSuite>::shared_from_this;
    string name = std:("project-manager");

    string description = std:("Tests for the project manager agent");

    any scenarioService;

    double completionTimeout;

    ProjectManagerTestSuite(double completionTimeout = 15000);
    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("Test Project Creation")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std:("Scenario service not found")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Project Setup Test"), std:("Test Client")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("project-setup")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("Create a new project for CRM implementation")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std:("Project creation timed out")));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Test Task Assignment")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std:("Scenario service not found")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Task Management Test"), std:("Test Team Lead")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("task-management")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("Assign UI development tasks to Alice and Bob")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std:("Task assignment timed out")));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Test Status Reporting")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std:("Scenario service not found")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Reporting Test"), std:("Test Stakeholder")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("reports")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("Generate weekly progress report for executive review")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std:("Status report generation timed out")));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Test Meeting Scheduling")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std:("Scenario service not found")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Scheduling Test"), std:("Test Coordinator")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("meetings")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("Schedule sprint planning meeting for next Monday")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std:("Meeting scheduling timed out")));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Test Off-Topic Handling")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std:("Scenario service not found")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("OffTopic Test"), std:("Test User")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("general")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("What's the weather forecast for tomorrow?")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std:("Off-topic handling timed out")));
        }
        }
    } };
};

#endif
