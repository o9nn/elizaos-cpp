#include "index.hpp"
#include <string>

any getGoalsService(std::shared_ptr<IAgentRuntime> runtime)
{
    return as<std::shared_ptr<GoalService>>((OR((runtime->getService(std::string("goals"))), (runtime->getService(std::string("goals"))))));
};


std::shared_ptr<Plugin> GoalsPlugin = object{
    object::pair{std::string("name"), std::string("goals")}, 
    object::pair{std::string("description"), std::string("Provides goal management functionality for tracking and achieving objectives.")}, 
    object::pair{std::string("providers"), array<any>{ goalsProvider }}, 
    object::pair{std::string("dependencies"), array<string>{ std::string("@elizaos/plugin-sql") }}, 
    object::pair{std::string("testDependencies"), array<string>{ std::string("@elizaos/plugin-sql") }}, 
    object::pair{std::string("actions"), array<any>{ createGoalAction, completeGoalAction, confirmGoalAction, updateGoalAction, cancelGoalAction }}, 
    object::pair{std::string("schema"), goalSchemaExport}, 
    object::pair{std::string("services"), array<GoalService>{ GoalService }}, 
    object::pair{std::string("routes"), std::string("routes")}, 
    object::pair{std::string("tests"), e2eTestSuites}, 
};

void Main(void)
{
}

MAIN
