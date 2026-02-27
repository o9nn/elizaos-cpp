#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-sql/src/__tests__/integration/seed/agent-seed.h"

std::shared_ptr<Agent> testAgent = object{
    object::pair{std:("name"), std:("Integration Test Agent")}, 
    object::pair{std:("username"), std:("test_agent")}, 
    object::pair{std:("bio"), std:("A test agent for integration tests")}, 
    object::pair{std:("enabled"), true}, 
    object::pair{std:("settings"), object{
        object::pair{std:("testSetting"), std:("test value")}
    }}, 
    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std:("updatedAt"), ((std::make_shared<Date>()))->getTime()}
};

void Main(void)
{
}

MAIN
