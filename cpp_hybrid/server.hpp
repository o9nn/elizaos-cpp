#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_API-CLIENT_SRC_SERVICES_SERVER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_API-CLIENT_SRC_SERVICES_SERVER_H
#include "core.h"
#include "../lib/base-client.h"
#include "../types/server.h"

class ServerService;

class ServerService : public BaseApiClient, public std::enable_shared_from_this<ServerService> {
public:
    using std::enable_shared_from_this<ServerService>::shared_from_this;
    virtual std::shared_ptr<Promise<std::shared_ptr<ServerHealth>>> checkHealth();
    virtual std::shared_ptr<Promise<object>> ping();
    virtual std::shared_ptr<Promise<object>> hello();
    virtual std::shared_ptr<Promise<std::shared_ptr<ServerStatus>>> getStatus();
    virtual std::shared_ptr<Promise<object>> stopServer();
    virtual std::shared_ptr<Promise<std::shared_ptr<ServerDebugInfo>>> getDebugInfo();
    virtual std::shared_ptr<Promise<object>> submitLogs(array<std::shared_ptr<LogSubmitParams>> logs);
    virtual std::shared_ptr<Promise<object>> clearLogs();
    ServerService(std::shared_ptr<ApiClientConfig> config);
};

#endif
