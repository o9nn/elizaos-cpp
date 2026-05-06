#include "..common/types.ts.hpp"
#include "..common/utils.ts.hpp"
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



struct TavilySearchResponse {
    Array<{ results;
    std::string title;
    std::string url;
    std::string content;
};

class TavilySearchPlugin implements SearchPlugin {
  readonly name: string = "tavily-search";
  readonly description: string = "Search the web using Tavily API";
  config: TavilyPluginConfig;
  private rateLimiter = createRateLimiter(60, 60000); // 60 requests per minute

  constructor(config: TavilyPluginConfig) {
    this.config = { ...DEFAULT_CONFIG, ...config };
    validateApiKey(this.config);
  }

          return handleApiError(error);


} // namespace elizaos
