#include "index.hpp"

std::shared_ptr<Plugin> formsPlugin = object{
    object::pair{std:("name"), std:("@elizaos/plugin-forms")}, 
    object::pair{std:("description"), std:("Structured form collection capabilities for conversational data gathering")}, 
    object::pair{std:("services"), array<FormsService>{ FormsService }}, 
    object::pair{std:("providers"), array<any>{ formsProvider }}, 
    object::pair{std:("actions"), array<any>{ createFormAction, updateFormAction, cancelFormAction }}, 
    object::pair{std:("schema"), formsSchema}, 
    object::pair{std:("evaluators"), array<any>()}, 
    object::pair{std:("tests"), array<std::shared_ptr<FormsPluginTestSuite>>{ FormsPluginTestSuite }}, 
    object::pair{std:("dependencies"), array<string>{ std:("@elizaos/plugin-sql") }}, 
    object::pair{std:("testDependencies"), array<string>{ std:("@elizaos/plugin-sql") }}
};

void Main(void)
{
}

MAIN
