#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/integration/seed/agent-seed.h"

std::shared_ptr<Agent> testAgent = object{
    object::pair{std::string("name"), std::string("Integration Test Agent")}, 
    object::pair{std::string("username"), std::string("test_agent")}, 
    object::pair{std::string("bio"), std::string("A test agent for integration tests")}, 
    object::pair{std::string("enabled"), true}, 
    object::pair{std::string("settings"), object{
        object::pair{std::string("testSetting"), std::string("test value")}
    }}, 
    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->getTime()}, 
    object::pair{std::string("updatedAt"), ((std::make_shared<Date>()))->getTime()}
};

void Main(void)
{
}

MAIN
