#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "services/coindeskService.hpp"
#include "services/tavilyService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



std::string MaxTokens(const std::string& data, double maxTokens = DEFAULT_MAX_CRYPTO_NEWS_CHARS);

            // Check for either CoinDesk or Tavily service

            // Calculate date range for CoinDesk API

            // Try CoinDesk API first if available and either no source specified or source is coindesk

                // CoinDesk API returned no results, log and fall through to Tavily

            // Fall back to Tavily with source filtering


} // namespace elizaos
