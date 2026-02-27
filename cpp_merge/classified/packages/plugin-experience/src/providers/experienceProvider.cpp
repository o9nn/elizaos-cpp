#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-experience/src/providers/experienceProvider.h"

std::shared_ptr<Provider> experienceProvider = object{
    object::pair{std:("name"), std:("EXPERIENCE")}, 
    object::pair{std:("description"), std:("Provides relevant past experiences and learnings for the current context")}, 
};

void Main(void)
{
}

MAIN
