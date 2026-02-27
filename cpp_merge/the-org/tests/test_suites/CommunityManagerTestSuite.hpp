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
    string name = std:("community-manager");

    string description = std:("Tests for the community manager agent");

    any scenarioService;

    double completionTimeout;

    CommunityManagerTestSuite(double completionTimeout = 10000);
    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("Test Conflict Resolution")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std:("Scenario service not found")));
            }
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Conflict Test"), std:("Test Owner")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("general")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
            auto userId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, userId); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("There's a user causing disruptions in the general channel")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) {
                throw any(std::make_shared<Error>(std:("Agent did not resolve conflict in time")));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Test New User Onboarding")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std:("Scenario service not found")));
            }
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Onboarding Test"), std:("Test Owner")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("welcome")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
            auto newUserId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, newUserId); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("Hi everyone, I'm new here!")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) {
                throw any(std::make_shared<Error>(std:("Agent did not complete onboarding in time")));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Test Moderation Actions")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std:("Scenario service not found")));
            }
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Moderation Test"), std:("Test Owner")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("moderation")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
            auto userId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, userId); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("This user posted inappropriate content")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) {
                throw any(std::make_shared<Error>(std:("Agent did not handle moderation in time")));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Test Community Engagement")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std:("Scenario service not found")));
            }
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Engagement Test"), std:("Test Owner")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("events")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
            auto userId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, userId); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("Let's plan the next community event")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) {
                throw any(std::make_shared<Error>(std:("Agent did not engage in time")));
            }
        }
        }
    } };
};

#endif
