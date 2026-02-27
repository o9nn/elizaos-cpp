#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_TESTS_TEST_SUITES_LIASIONTESTSUITE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_TESTS_TEST_SUITES_LIASIONTESTSUITE_H
#include "core.h"
#include "@elizaos/core.h"
#include "uuid.h"
using uuidv4 = v4;

class LiaisonTestSuite;

class LiaisonTestSuite : public TestSuite, public std::enable_shared_from_this<LiaisonTestSuite> {
public:
    using std::enable_shared_from_this<LiaisonTestSuite>::shared_from_this;
    string name = std:("liaison");

    string description = std:("Tests for the community liaison agent");

    any scenarioService;

    double completionTimeout;

    LiaisonTestSuite(double completionTimeout = 10000);
    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("Test Platform Information Request")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std:("Scenario service not found")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Platform Info Test"), std:("Test User")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("general")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("What's being discussed in the Telegram group about ElizaOS?")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std:("Platform info response timed out")));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Test Channel Recommendation")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std:("Scenario service not found")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Channel Test"), std:("Test User")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("support")); });
            auto response = std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("Where should I ask about agent deployment?")); });
            if (!response) throw any(std::make_shared<Error>(std:("sendMessage did not return a valid response")));
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std:("Channel recommendation timed out")));
            return response;
        }
        }
    }, object{
        object::pair{std:("name"), std:("Test Cross-Platform Coordination")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std:("Scenario service not found")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Coordination Test"), std:("Test Organizer")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("events")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("Can you help coordinate a cross-platform hackathon?")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std:("Coordination response timed out")));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Test Announcement Management")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std:("Scenario service not found")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Announcement Test"), std:("Test Admin")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("announcements")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("Notify all platforms about the upcoming maintenance")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std:("Announcement handling timed out")));
        }
        }
    }, object{
        object::pair{std:("name"), std:("Test Ignore Off-Topic")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std:("Scenario service not found")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Ignore Test"), std:("Test User")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("general")); });
            auto response = std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("I'm feeling anxious about my project deadline")); });
            if (!response) return nullptr;
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std:("Ignore handling timed out")));
            return response;
        }
        }
    } };
};

#endif
