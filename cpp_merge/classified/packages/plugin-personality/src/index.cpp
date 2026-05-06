#include "index.hpp"
#include <string>

std::shared_ptr<Plugin> selfModificationPlugin = object{
    object::pair{std::string("name"), std::string("@elizaos/plugin-personality")}, 
    object::pair{std::string("description"), std::string("Enables agent self-modification and character evolution through conversation analysis and user feedback")}, 
    object::pair{std::string("evaluators"), array<any>{ characterEvolutionEvaluator }}, 
    object::pair{std::string("actions"), array<any>{ modifyCharacterAction }}, 
    object::pair{std::string("services"), array<CharacterFileManager>{ CharacterFileManager }}, 
    object::pair{std::string("config"), object{
        object::pair{std::string("EVOLUTION_COOLDOWN_MS"), 5 * 60 * 1000}, 
        object::pair{std::string("MODIFICATION_CONFIDENCE_THRESHOLD"), 0.7}, 
        object::pair{std::string("MAX_BIO_ELEMENTS"), 20}, 
        object::pair{std::string("MAX_TOPICS"), 50}, 
        object::pair{std::string("MAX_BACKUPS"), 10}, 
        object::pair{std::string("REQUIRE_ADMIN_APPROVAL"), false}, 
        object::pair{std::string("ENABLE_AUTO_EVOLUTION"), true}, 
        object::pair{std::string("VALIDATE_MODIFICATIONS"), true}, 
        object::pair{std::string("BACKUP_DIRECTORY"), std::string(".eliza/character-backups")}, 
        object::pair{std::string("CHARACTER_FILE_DETECTION"), true}
    }}, 
};

void Main(void)
{
}

MAIN
