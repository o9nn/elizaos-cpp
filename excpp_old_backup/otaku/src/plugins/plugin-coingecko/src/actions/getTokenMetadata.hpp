#include ".services/coingecko.service.hpp"
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



double normalizeConfidence(double confidence);

std::string formatConfidencePercentage(double confidence);

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
