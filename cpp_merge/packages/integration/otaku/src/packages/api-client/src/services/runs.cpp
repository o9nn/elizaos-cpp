#include "runs.hpp"

std::shared_ptr<Promise<object>> RunsService::listRuns(std::shared_ptr<UUID> agentId, std::shared_ptr<ListRunsParams> params)
{
    return this->get<object>(std:("/api/agents/") + agentId + std:("/runs"), object{
        object::pair{std:("params"), std:("params")}
    });
}

std::shared_ptr<Promise<std::shared_ptr<RunDetail>>> RunsService::getRun(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> runId, std::shared_ptr<UUID> roomId)
{
    return this->get<std::shared_ptr<RunDetail>>(std:("/api/agents/") + agentId + std:("/runs/") + runId + string_empty, object{
        object::pair{std:("params"), (roomId) ? any(object{
            object::pair{std:("roomId"), std:("roomId")}
        }) (undefined)}
    });
}

RunsService::RunsService(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}

