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



struct ExaSearchResponse {
    Array<{ results;
    std::string title;
    std::string url;
    std::string snippet;
    double score;
};

class ExaSearchPlugin implements SearchPlugin {
  name = "exa-search";
  description = "Search using Exa API for semantic, code, and document search";
  config: ExaPluginConfig;
  private rateLimiter = createRateLimiter(60, 60000); // 60 requests per minute

  constructor(config: ExaPluginConfig) {
    this.config = { ...DEFAULT_CONFIG, ...config };
    validateApiKey(this.config);
  }

          return handleApiError(error);


} // namespace elizaos
