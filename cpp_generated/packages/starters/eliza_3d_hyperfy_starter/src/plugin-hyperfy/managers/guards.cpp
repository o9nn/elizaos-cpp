#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-3d-hyperfy-starter/src/plugin-hyperfy/managers/guards.h"

boolean AgentActivityLock::isActive()
{
    return this->count > 0;
}

void AgentActivityLock::enter()
{
    this->count++;
}

void AgentActivityLock::exit()
{
    this->count = Math->max(0, this->count - 1);
}

std::shared_ptr<AgentActivityLock> agentActivityLock = std::make_shared<AgentActivityLock>();

void Main(void)
{
}

MAIN
