#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_TESTS_SUITE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_TESTS_SUITE_H
#include "core.h"
#include "@elizaos/core.h"
#include "uuid.h"
using uuidv4 = v4;

class SpartanTestSuite;

class SpartanTestSuite : public TestSuite, public std::enable_shared_from_this<SpartanTestSuite> {
public:
    using std::enable_shared_from_this<SpartanTestSuite>::shared_from_this;
    string name = std::string("investment-manager");

    string description = std::string("Tests for the investment manager agent");

    any scenarioService;

    double completionTimeout;

    SpartanTestSuite(double completionTimeout = 15000);
    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("Test Portfolio Rebalancing")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            }
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Portfolio Test"), std::string("Test Investor")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("trading")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
            auto userId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, userId); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("Rebalance portfolio to 60% equities and 40% bonds")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) {
                throw any(std::make_shared<Error>(std::string("Portfolio rebalancing timed out")));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Trade Execution")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            }
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Trading Test"), std::string("Test Trader")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("execution")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
            auto userId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, userId); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("Execute buy order for 1000 shares of AAPL at market price")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) {
                throw any(std::make_shared<Error>(std::string("Trade execution timed out")));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Risk Assessment")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            }
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Risk Test"), std::string("Test Analyst")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("analysis")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
            auto userId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, userId); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("Analyze portfolio risk exposure and suggest mitigation strategies")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) {
                throw any(std::make_shared<Error>(std::string("Risk assessment timed out")));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Test Compliance Check")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std::string("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std::string("Scenario service not found")));
            }
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std::string("Compliance Test"), std::string("Test Auditor")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std::string("regulatory")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
            auto userId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, userId); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std::string("Verify all trades comply with SEC regulations")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) {
                throw any(std::make_shared<Error>(std::string("Compliance check timed out")));
            }
        }
        }
    } };
};

#endif
