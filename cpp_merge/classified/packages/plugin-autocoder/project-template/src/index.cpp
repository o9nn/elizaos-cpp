#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/project-template/src/index.h"

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
