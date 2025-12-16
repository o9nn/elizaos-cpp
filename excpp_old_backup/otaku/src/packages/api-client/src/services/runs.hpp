#include ".lib/base-client.hpp"
#include ".types/runs.hpp"
#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class RunsService extends BaseApiClient {
  async listRuns(
    agentId: UUID,
    params?: ListRunsParams
  ): Promise<{ runs: RunSummary[]; total: number; hasMore: boolean }> {
    return this.get<{ runs: RunSummary[]; total: number; hasMore: boolean }>(
      `/api/agents/${agentId}/runs`,
      { params }
    );
  }


} // namespace elizaos
