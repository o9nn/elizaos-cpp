#include "index.hpp"
#include <string>

std::shared_ptr<ProjectAgent> projectAgent = object{
    object::pair{std::string("character"), std::string("character")}
};
std::shared_ptr<Project> project = object{
    object::pair{std::string("agents"), array<any>{ projectAgent }}
};

void Main(void)
{
}

MAIN
