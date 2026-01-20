#include "workflow-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

WorkflowUtils createWorkflowUtils() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const WorkflowConfig config = {;
        sessionLogsEndpoint: process.env.SESSION_LOGS_ENDPOINT || std::nullopt,
        enableSessionLogging: process.env.ENABLE_SESSION_LOGGING != "false",
        failOnSessionLogError: process.env.FAIL_ON_SESSION_LOG_ERROR == "true",
        retryCount: parseInt(process.env.SESSION_LOG_RETRY_COUNT || "3", 10),
        timeoutMs: parseInt(process.env.SESSION_LOG_TIMEOUT_MS || "10000", 10)
        };

        return new WorkflowUtils(config);

}

} // namespace elizaos
