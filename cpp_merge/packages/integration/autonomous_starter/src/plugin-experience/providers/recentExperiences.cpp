#include "recentExperiences.hpp"

std::shared_ptr<Provider> recentExperiencesProvider = object{
    object::pair{std:("name"), std:("recentExperiences")}, 
    object::pair{std:("description"), std:("Provides recent experiences, statistics, and detected patterns")}, 
};

void Main(void)
{
}

MAIN
