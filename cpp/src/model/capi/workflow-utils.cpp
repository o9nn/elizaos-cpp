#include "workflow-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

WorkflowUtils createWorkflowUtils() {
    WorkflowConfig config;
    
    config.sessionLogsEndpoint = getEnvVar("SESSION_LOGS_ENDPOINT");
    
    auto enableLogging = getEnvVar("ENABLE_SESSION_LOGGING");
    config.enableSessionLogging = !enableLogging.has_value() || enableLogging.value() != "false";
    
    auto failOnError = getEnvVar("FAIL_ON_SESSION_LOG_ERROR");
    config.failOnSessionLogError = failOnError.has_value() && failOnError.value() == "true";
    
    config.retryCount = parseIntEnv("SESSION_LOG_RETRY_COUNT", 3);
    config.timeoutMs = parseIntEnv("SESSION_LOG_TIMEOUT_MS", 10000);

    return WorkflowUtils(config);
}

} // namespace elizaos
