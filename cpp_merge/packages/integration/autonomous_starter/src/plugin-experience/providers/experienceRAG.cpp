#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-experience/providers/experienceRAG.h"

std::shared_ptr<Provider> experienceRAGProvider = object{
    object::pair{std:("name"), std:("experienceRAG")}, 
    object::pair{std:("description"), std:("Searches past experiences for relevant learnings and insights")}, 
};

void Main(void)
{
}

MAIN
