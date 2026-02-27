#include "recentExperiences.hpp"
#include <string>

std::shared_ptr<Provider> recentExperiencesProvider = object{
    object::pair{std::string("name"), std::string("recentExperiences")}, 
    object::pair{std::string("description"), std::string("Provides recent experiences, statistics, and detected patterns")}, 
};

void Main(void)
{
}

MAIN
