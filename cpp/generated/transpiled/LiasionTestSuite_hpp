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
    string name = std::string("liaison");

    string description = std::string("Tests for the community liaison agent");

    any scenarioService;

    double completionTimeout;

    LiaisonTestSuite(double completionTimeout = 10000);
    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("Test Platform Information Request")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Platform Info Test"), std::string("Test User")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("general")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("What's being discussed in the Telegram group about ElizaOS?")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std::string("Platform info response timed out")));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Channel Recommendation")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Channel Test"), std::string("Test User")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("support")); });
            auto response = std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("Where should I ask about agent deployment?")); });
            if (!response) throw any(std::make_shared<Error>(std::string("sendMessage did not return a valid response")));
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std::string("Channel recommendation timed out")));
            return response;
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Cross-Platform Coordination")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Coordination Test"), std::string("Test Organizer")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("events")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("Can you help coordinate a cross-platform hackathon?")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std::string("Coordination response timed out")));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Announcement Management")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Announcement Test"), std::string("Test Admin")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("announcements")); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("Notify all platforms about the upcoming maintenance")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std::string("Announcement handling timed out")));
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Ignore Off-Topic")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Ignore Test"), std::string("Test User")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("general")); });
            auto response = std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("I'm feeling anxious about my project deadline")); });
            if (!response) return nullptr;
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) throw any(std::make_shared<Error>(std::string("Ignore handling timed out")));
            return response;
        }
        }
    } };
};

#endif
