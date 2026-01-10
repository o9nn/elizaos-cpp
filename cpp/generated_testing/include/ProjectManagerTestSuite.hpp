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
    string name = std::string("project-manager");

    string description = std::string("Tests for the project manager agent");

    any scenarioService;

    double completionTimeout;

    ProjectManagerTestSuite(double completionTimeout = 15000);
    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("Test Project Creation")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Project Setup Test"), std::string("Test Client")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("project-setup")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("Create a new project for CRM implementation")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std::string("Project creation timed out")));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Task Assignment")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Task Management Test"), std::string("Test Team Lead")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("task-management")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("Assign UI development tasks to Alice and Bob")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std::string("Task assignment timed out")));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Status Reporting")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Reporting Test"), std::string("Test Stakeholder")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("reports")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("Generate weekly progress report for executive review")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std::string("Status report generation timed out")));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Meeting Scheduling")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Scheduling Test"), std::string("Test Coordinator")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("meetings")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("Schedule sprint planning meeting for next Monday")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std::string("Meeting scheduling timed out")));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Off-Topic Handling")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("OffTopic Test"), std::string("Test User")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("general")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("What's the weather forecast for tomorrow?")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std::string("Off-topic handling timed out")));
        }
        }
    } };
};

#endif
