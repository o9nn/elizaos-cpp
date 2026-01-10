#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_API-CLIENT_SRC_SERVICES_RUNS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_API-CLIENT_SRC_SERVICES_RUNS_H
#include "core.h"
#include "@elizaos/core.h"
#include "../lib/base-client.h"
#include "../types/runs.h"

class RunsService;

class RunsService : public BaseApiClient, public std::enable_shared_from_this<RunsService> {
public:
    using std::enable_shared_from_this<RunsService>::shared_from_this;
    virtual std::shared_ptr<Promise<object>> listRuns(std::shared_ptr<UUID> agentId, std::shared_ptr<ListRunsParams> params = undefined);
    virtual std::shared_ptr<Promise<std::shared_ptr<RunDetail>>> getRun(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> runId, std::shared_ptr<UUID> roomId = undefined);
    RunsService(std::shared_ptr<ApiClientConfig> config);
};

#endif
