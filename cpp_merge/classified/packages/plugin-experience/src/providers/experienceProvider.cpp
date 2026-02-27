#include "experienceProvider.hpp"

std::shared_ptr<Provider> experienceProvider = object{
    object::pair{std:("name"), std:("EXPERIENCE")}, 
    object::pair{std:("description"), std:("Provides relevant past experiences and learnings for the current context")}, 
};

void Main(void)
{
}

MAIN
