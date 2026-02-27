#include "index.hpp"

std::shared_ptr<Plugin> autocoderPlugin = object{
    object::pair{std:("name"), std:("@elizaos/plugin-autocoder")}, 
    object::pair{std:("description"), std:("Advanced code generation plugin that enables autonomous plugin development. Searches registry, generates PRDs, and creates working plugins with validation.")}, 
    object::pair{std:("services"), array<CodeGenerationService>{ CodeGenerationService, ProjectPlanningService, SecretsManagerService, ProjectStatusManager }}, 
    object::pair{std:("actions"), array<any>{ generateCodeAction, createProjectAction }}, 
    object::pair{std:("providers"), array<any>{ projectsProvider, currentProjectProvider }}, 
    object::pair{std:("dependencies"), array<string>{ std:("@elizaos/plugin-forms"), std:("@elizaos/plugin-plugin-manager") }}, 
    object::pair{std:("testDependencies"), array<string>{ std:("@elizaos/plugin-forms") }}, 
    object::pair{std:("tests"), testSuites}
};

void Main(void)
{
}

MAIN
