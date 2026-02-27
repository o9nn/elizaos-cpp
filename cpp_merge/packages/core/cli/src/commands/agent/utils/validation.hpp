#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_AGENT_UTILS_VALIDATION_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_AGENT_UTILS_VALIDATION_H
#include "core.hpp"
#include "commander.hpp"
#include "zod.hpp"
#include "../../shared.h"
#include "../../shared.h"

extern any AgentBasicSchema;
extern any AgentsListResponseSchema;
std::shared_ptr<Promise<array<std::shared_ptr<AgentBasic>>>> getAgents(std::shared_ptr<OptionValues> opts);

std::shared_ptr<Promise<string>> resolveAgentId(string idOrNameOrIndex, std::shared_ptr<OptionValues> opts);

#endif
