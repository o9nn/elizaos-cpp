#include "experienceProvider.hpp"
#include <string>

std::shared_ptr<Provider> experienceProvider = object{
    object::pair{std::string("name"), std::string("EXPERIENCE")}, 
    object::pair{std::string("description"), std::string("Provides relevant past experiences and learnings for the current context")}, 
};

void Main(void)
{
}

MAIN
