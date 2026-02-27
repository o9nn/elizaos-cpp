#include "index.hpp"

std::shared_ptr<Plugin> selfModificationPlugin = object{
    object::pair{std:("name"), std:("@elizaos/plugin-personality")}, 
    object::pair{std:("description"), std:("Enables agent self-modification and character evolution through conversation analysis and user feedback")}, 
    object::pair{std:("evaluators"), array<any>{ characterEvolutionEvaluator }}, 
    object::pair{std:("actions"), array<any>{ modifyCharacterAction }}, 
    object::pair{std:("services"), array<CharacterFileManager>{ CharacterFileManager }}, 
    object::pair{std:("config"), object{
        object::pair{std:("EVOLUTION_COOLDOWN_MS"), 5 * 60 * 1000}, 
        object::pair{std:("MODIFICATION_CONFIDENCE_THRESHOLD"), 0.7}, 
        object::pair{std:("MAX_BIO_ELEMENTS"), 20}, 
        object::pair{std:("MAX_TOPICS"), 50}, 
        object::pair{std:("MAX_BACKUPS"), 10}, 
        object::pair{std:("REQUIRE_ADMIN_APPROVAL"), false}, 
        object::pair{std:("ENABLE_AUTO_EVOLUTION"), true}, 
        object::pair{std:("VALIDATE_MODIFICATIONS"), true}, 
        object::pair{std:("BACKUP_DIRECTORY"), std:(".eliza/character-backups")}, 
        object::pair{std:("CHARACTER_FILE_DETECTION"), true}
    }}, 
};

void Main(void)
{
}

MAIN
