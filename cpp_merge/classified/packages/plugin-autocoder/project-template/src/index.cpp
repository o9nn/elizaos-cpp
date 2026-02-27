#include "index.hpp"

std::shared_ptr<ProjectAgent> projectAgent = object{
    object::pair{std:("character"), std:("character")}
};
std::shared_ptr<Project> project = object{
    object::pair{std:("agents"), array<any>{ projectAgent }}
};

void Main(void)
{
}

MAIN
