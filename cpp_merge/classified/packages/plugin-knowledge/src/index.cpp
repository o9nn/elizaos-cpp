#include "index.hpp"
#include <string>

std::shared_ptr<Plugin> knowledgePlugin = object{
    object::pair{std::string("name"), std::string("knowledge")}, 
    object::pair{std::string("description"), std::string("Plugin for Retrieval Augmented Generation, including knowledge management and embedding.")}, 
    object::pair{std::string("services"), array<KnowledgeService>{ KnowledgeService }}, 
    object::pair{std::string("providers"), array<any>{ knowledgeProvider }}, 
    object::pair{std::string("routes"), knowledgeRoutes}, 
    object::pair{std::string("actions"), knowledgeActions}, 
    object::pair{std::string("tests"), array<std::shared_ptr<KnowledgeTestSuite>>{ knowledgeTestSuite }}
};

void Main(void)
{
}

MAIN
