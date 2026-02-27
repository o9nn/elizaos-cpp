#include "index.hpp"

any getGoalsService(std::shared_ptr<IAgentRuntime> runtime)
{
    return as<std::shared_ptr<GoalService>>((OR((runtime->getService(std:("goals"))), (runtime->getService(std:("goals"))))));
};


std::shared_ptr<Plugin> GoalsPlugin = object{
    object::pair{std:("name"), std:("goals")}, 
    object::pair{std:("description"), std:("Provides goal management functionality for tracking and achieving objectives.")}, 
    object::pair{std:("providers"), array<any>{ goalsProvider }}, 
    object::pair{std:("dependencies"), array<string>{ std:("@elizaos/plugin-sql") }}, 
    object::pair{std:("testDependencies"), array<string>{ std:("@elizaos/plugin-sql") }}, 
    object::pair{std:("actions"), array<any>{ createGoalAction, completeGoalAction, confirmGoalAction, updateGoalAction, cancelGoalAction }}, 
    object::pair{std:("schema"), goalSchemaExport}, 
    object::pair{std:("services"), array<GoalService>{ GoalService }}, 
    object::pair{std:("routes"), std:("routes")}, 
    object::pair{std:("tests"), e2eTestSuites}, 
};

void Main(void)
{
}

MAIN
