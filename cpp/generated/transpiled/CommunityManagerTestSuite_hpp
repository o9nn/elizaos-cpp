#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_TESTS_TEST_SUITES_COMMUNITYMANAGERTESTSUITE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_TESTS_TEST_SUITES_COMMUNITYMANAGERTESTSUITE_H
#include "core.h"
#include "@elizaos/core.h"
#include "uuid.h"
using uuidv4 = v4;

class CommunityManagerTestSuite;

class CommunityManagerTestSuite : public TestSuite, public std::enable_shared_from_this<CommunityManagerTestSuite> {
public:
    using std::enable_shared_from_this<CommunityManagerTestSuite>::shared_from_this;
    string name = std::string("community-manager");

    string description = std::string("Tests for the community manager agent");

    any scenarioService;

    double completionTimeout;

    CommunityManagerTestSuite(double completionTimeout = 10000);
    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("Test Conflict Resolution")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            }
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Conflict Test"), std::string("Test Owner")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("general")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
            auto userId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, userId); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("There's a user causing disruptions in the general channel")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) {
                throw any(std::make_shared<Error>(std::string("Agent did not resolve conflict in time")));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test New User Onboarding")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            }
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Onboarding Test"), std::string("Test Owner")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("welcome")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
            auto newUserId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, newUserId); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("Hi everyone, I'm new here!")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) {
                throw any(std::make_shared<Error>(std::string("Agent did not complete onboarding in time")));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Moderation Actions")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            }
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Moderation Test"), std::string("Test Owner")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("moderation")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
            auto userId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, userId); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("This user posted inappropriate content")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) {
                throw any(std::make_shared<Error>(std::string("Agent did not handle moderation in time")));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Community Engagement")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            }
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Engagement Test"), std::string("Test Owner")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("events")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
            auto userId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, userId); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("Let's plan the next community event")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) {
                throw any(std::make_shared<Error>(std::string("Agent did not engage in time")));
            }
        }
        }
    } };
};

#endif
