#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/api-client/src/services/server.h"

std::shared_ptr<Promise<std::shared_ptr<ServerHealth>>> ServerService::checkHealth()
{
    return this->get<std::shared_ptr<ServerHealth>>(std::string("/api/server/health"));
}

std::shared_ptr<Promise<object>> ServerService::ping()
{
    return this->get<object>(std::string("/api/server/ping"));
}

std::shared_ptr<Promise<object>> ServerService::hello()
{
    return this->get<object>(std::string("/api/server/hello"));
}

std::shared_ptr<Promise<std::shared_ptr<ServerStatus>>> ServerService::getStatus()
{
    return this->get<std::shared_ptr<ServerStatus>>(std::string("/api/server/status"));
}

std::shared_ptr<Promise<object>> ServerService::stopServer()
{
    return this->post<object>(std::string("/api/server/stop"));
}

std::shared_ptr<Promise<std::shared_ptr<ServerDebugInfo>>> ServerService::getDebugInfo()
{
    return this->get<std::shared_ptr<ServerDebugInfo>>(std::string("/api/server/debug/servers"));
}

std::shared_ptr<Promise<object>> ServerService::submitLogs(array<std::shared_ptr<LogSubmitParams>> logs)
{
    return this->post<object>(std::string("/api/server/logs"), object{
        object::pair{std::string("logs"), std::string("logs")}
    });
}

std::shared_ptr<Promise<object>> ServerService::clearLogs()
{
    return this->delete<object>(std::string("/api/server/logs"));
}

ServerService::ServerService(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}

