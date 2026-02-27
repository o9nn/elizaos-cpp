#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_TESTS_SUITE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_TESTS_SUITE_H
#include "core.hpp"
// External dependency removed
#include "uuid.hpp"
using uuidv4 = v4;

class SpartanTestSuite;

class SpartanTestSuite : public TestSuite, public std::enable_shared_from_this<SpartanTestSuite> {
public:
    using std::enable_shared_from_this<SpartanTestSuite>::shared_from_this;
    string name = std:("investment-manager");

    string description = std:("Tests for the investment manager agent");

    any scenarioService;

    double completionTimeout;

    SpartanTestSuite(double completionTimeout = 15000);
    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("Test Portfolio Rebalancing")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std:("Scenario service not found")));
            }
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Portfolio Test"), std:("Test Investor")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("trading")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
            auto userId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, userId); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("Rebalance portfolio to 60% equities and 40% bonds")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) {
                throw any(std::make_shared<Error>(std:("Portfolio rebalancing timed out")));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Test Trade Execution")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std:("Scenario service not found")));
            }
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Trading Test"), std:("Test Trader")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("execution")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
            auto userId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, userId); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("Execute buy order for 1000 shares of AAPL at market price")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) {
                throw any(std::make_shared<Error>(std:("Trade execution timed out")));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Test Risk Assessment")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std:("Scenario service not found")));
            }
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Risk Test"), std:("Test Analyst")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("analysis")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
            auto userId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, userId); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("Analyze portfolio risk exposure and suggest mitigation strategies")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) {
                throw any(std::make_shared<Error>(std:("Risk assessment timed out")));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Test Compliance Check")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            this->scenarioService = runtime->getService(std:("scenario"));
            if (!this->scenarioService) {
                throw any(std::make_shared<Error>(std:("Scenario service not found")));
            }
            auto worldId = std::async([=]() { this->scenarioService["createWorld"](std:("Compliance Test"), std:("Test Auditor")); });
            auto roomId = std::async([=]() { this->scenarioService["createRoom"](worldId, std:("regulatory")); });
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, runtime->agentId); });
            auto userId = as<std::shared_ptr<UUID>>(uuidv4());
            std::async([=]() { this->scenarioService["addParticipant"](worldId, roomId, userId); });
            std::async([=]() { this->scenarioService["sendMessage"](runtime, worldId, roomId, std:("Verify all trades comply with SEC regulations")); });
            auto completed = std::async([=]() { this->scenarioService["waitForCompletion"](this->completionTimeout); });
            if (!completed) {
                throw any(std::make_shared<Error>(std:("Compliance check timed out")));
            }
        }
        }
    } };
};

#endif
