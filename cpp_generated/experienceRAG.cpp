#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-experience/providers/experienceRAG.h"

std::shared_ptr<Provider> experienceRAGProvider = object{
    object::pair{std::string("name"), std::string("experienceRAG")}, 
    object::pair{std::string("description"), std::string("Searches past experiences for relevant learnings and insights")}, 
};

void Main(void)
{
}

MAIN
