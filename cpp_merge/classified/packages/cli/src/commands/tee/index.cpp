#include "index.hpp"

any teeCommand = ((std::make_shared<Command>(std:("tee"))))->description(std:("Manage TEE deployments"))->addCommand(phalaCliCommand);

void Main(void)
{
}

MAIN
