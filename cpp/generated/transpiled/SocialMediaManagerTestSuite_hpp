#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_TESTS_TEST_SUITES_SOCIALMEDIAMANAGERTESTSUITE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_TESTS_TEST_SUITES_SOCIALMEDIAMANAGERTESTSUITE_H
#include "core.h"
#include "@elizaos/core.h"
#include "uuid.h"
using uuidv4 = v4;

class SocialMediaManagerTestSuite;

class SocialMediaManagerTestSuite : public TestSuite, public std::enable_shared_from_this<SocialMediaManagerTestSuite> {
public:
    using std::enable_shared_from_this<SocialMediaManagerTestSuite>::shared_from_this;
    string name = std::string("social-media-manager");

    string description = std::string("Tests for the social media manager agent");

    any scenarioService;

    double completionTimeout;

    SocialMediaManagerTestSuite(double completionTimeout = 10000);
    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("Test Onboarding Process")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            }
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Test Organization"), std::string("Test Owner")); });
            auto mainRoomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("general")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, mainRoomId, runtime->agentId); });
            auto ownerId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, mainRoomId, ownerId); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, mainRoomId, std::string("Hi! I'd like to set up social media management for my organization.")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) {
                throw any(std::make_shared<Error>(std::string("Agent did not complete onboarding response in time")));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Cross-Platform Post Creation")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            }
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Cross-Platform Test"), std::string("Test Owner")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("social-media")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
            auto ownerId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, ownerId); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("Please create a post about our new product launch for Twitter and Discord")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) {
                throw any(std::make_shared<Error>(std::string("Agent did not complete post creation in time")));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Response to User Queries")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            }
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Query Test"), std::string("Test Owner")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("help")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
            auto ownerId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, ownerId); });
            auto queries = array<string>{ std::string("What social media platforms do you support?"), std::string("Can you help me schedule posts?"), std::string("How do you handle engagement metrics?") };
            for (auto& query : queries)
            {
                std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, query); });
                auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
                if (!completed) {
                    throw any(std::make_shared<Error>(std::string("Agent did not respond to query in time: ") + query + string_empty));
                }
            }
        }
        }
    } };
};

#endif
