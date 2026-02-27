#include "index.hpp"

std::shared_ptr<Plugin> knowledgePlugin = object{
    object::pair{std:("name"), std:("knowledge")}, 
    object::pair{std:("description"), std:("Plugin for Retrieval Augmented Generation, including knowledge management and embedding.")}, 
    object::pair{std:("services"), array<KnowledgeService>{ KnowledgeService }}, 
    object::pair{std:("providers"), array<any>{ knowledgeProvider }}, 
    object::pair{std:("routes"), knowledgeRoutes}, 
    object::pair{std:("actions"), knowledgeActions}, 
    object::pair{std:("tests"), array<std::shared_ptr<KnowledgeTestSuite>>{ knowledgeTestSuite }}
};

void Main(void)
{
}

MAIN
