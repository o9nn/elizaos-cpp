#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/tee/index.h"

any teeCommand = ((std::make_shared<Command>(std::string("tee"))))->description(std::string("Manage TEE deployments"))->addCommand(phalaCliCommand);

void Main(void)
{
}

MAIN
