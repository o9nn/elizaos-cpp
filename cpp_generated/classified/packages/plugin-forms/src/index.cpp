#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-forms/src/index.h"

std::shared_ptr<Plugin> formsPlugin = object{
    object::pair{std::string("name"), std::string("@elizaos/plugin-forms")}, 
    object::pair{std::string("description"), std::string("Structured form collection capabilities for conversational data gathering")}, 
    object::pair{std::string("services"), array<FormsService>{ FormsService }}, 
    object::pair{std::string("providers"), array<any>{ formsProvider }}, 
    object::pair{std::string("actions"), array<any>{ createFormAction, updateFormAction, cancelFormAction }}, 
    object::pair{std::string("schema"), formsSchema}, 
    object::pair{std::string("evaluators"), array<any>()}, 
    object::pair{std::string("tests"), array<std::shared_ptr<FormsPluginTestSuite>>{ FormsPluginTestSuite }}, 
    object::pair{std::string("dependencies"), array<string>{ std::string("@elizaos/plugin-sql") }}, 
    object::pair{std::string("testDependencies"), array<string>{ std::string("@elizaos/plugin-sql") }}
};

void Main(void)
{
}

MAIN
