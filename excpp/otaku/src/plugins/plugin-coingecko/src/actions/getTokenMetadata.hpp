#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".services/coingecko.service.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



double normalizeConfidence(double confidence);

std::string formatConfidencePercentage(double confidence);

std::string extractAttribute(const std::unordered_map<std::string, unknown>& metadata, const std::variant<"name", "symbol">& key);

std::string describeCandidate(TokenMetadataCandidate candidate);

std::string summarizeResolution(TokenMetadataResolution result);

  // Parameter schema for tool calling

      // Read parameters from state (extracted by multiStepDecisionTemplate)

      // Extract and validate tokens parameter (required)

      // Parse comma-separated tokens

      // Store input parameters for return

      // Fetch token metadata

      // Try to capture input params even in failure


} // namespace elizaos
