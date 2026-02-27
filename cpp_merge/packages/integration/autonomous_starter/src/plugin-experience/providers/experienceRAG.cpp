#include "experienceRAG.hpp"

std::shared_ptr<Provider> experienceRAGProvider = object{
    object::pair{std:("name"), std:("experienceRAG")}, 
    object::pair{std:("description"), std:("Searches past experiences for relevant learnings and insights")}, 
};

void Main(void)
{
}

MAIN
