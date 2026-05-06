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



using TavilyClient = ReturnType<typeof tavily>;

class TavilyService extends Service implements ITavilyService {
    static serviceType = "TAVILY" as const;

    private tavilyClient!: TavilyClient;

    constructor(runtime: IAgentRuntime) {
        super(runtime);
    }

        // No persistent connections to close for Tavily client


} // namespace elizaos
