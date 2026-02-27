#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-self-modification/src/index.h"

std::shared_ptr<Plugin> selfModificationPlugin = object{
    object::pair{std::string("name"), std::string("self-modification")}, 
    object::pair{std::string("description"), std::string("Enables agents to modify their own character through reflection and self-learning")}, 
    object::pair{std::string("services"), array<CharacterModificationService>{ CharacterModificationService }}, 
    object::pair{std::string("providers"), array<any>{ characterStateProvider, characterDiffProvider }}, 
    object::pair{std::string("actions"), array<any>{ modifyCharacterAction, viewCharacterHistoryAction, rollbackCharacterAction }}, 
    object::pair{std::string("evaluators"), array<any>{ characterEvolutionEvaluator }}
};

void Main(void)
{
}

MAIN
