#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_AUTONOMOUS-TRADER_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_AUTONOMOUS-TRADER_UTILS_H
#include "core.h"
#include "@elizaos/core.h"

any acquireService(std::shared_ptr<IAgentRuntime> runtime, any serviceType, string asking = string_empty, double retries = 10);

any askLlmObject(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Object> ask, array<string> requiredFields, double maxRetries = 3);

any messageReply(any runtime, any message, any reply, any responses);

any takeItPrivate(any runtime, any message, any reply);

#endif
