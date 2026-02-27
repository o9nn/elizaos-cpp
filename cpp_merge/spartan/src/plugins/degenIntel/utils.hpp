#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_DEGENINTEL_UTILS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_DEGENINTEL_UTILS_H
#include "core.hpp"
// External dependency removed

any acquireService(std::shared_ptr<IAgentRuntime> runtime, any serviceType, string asking = string_empty, double retries = 10);

any askLlmObject(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Object> ask, array<string> requiredFields, double maxRetries = 3);

#endif
