#include "index.hpp"
#include <string>

any teeCommand = ((std::make_shared<Command>(std::string("tee"))))->description(std::string("Manage TEE deployments"))->addCommand(phalaCliCommand);

void Main(void)
{
}

MAIN
