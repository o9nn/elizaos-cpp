#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/api-client/src/services/runs.h"

std::shared_ptr<Promise<object>> RunsService::listRuns(std::shared_ptr<UUID> agentId, std::shared_ptr<ListRunsParams> params)
{
    return this->get<object>(std::string("/api/agents/") + agentId + std::string("/runs"), object{
        object::pair{std::string("params"), std::string("params")}
    });
}

std::shared_ptr<Promise<std::shared_ptr<RunDetail>>> RunsService::getRun(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> runId, std::shared_ptr<UUID> roomId)
{
    return this->get<std::shared_ptr<RunDetail>>(std::string("/api/agents/") + agentId + std::string("/runs/") + runId + string_empty, object{
        object::pair{std::string("params"), (roomId) ? any(object{
            object::pair{std::string("roomId"), std::string("roomId")}
        }) : any(undefined)}
    });
}

RunsService::RunsService(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}

