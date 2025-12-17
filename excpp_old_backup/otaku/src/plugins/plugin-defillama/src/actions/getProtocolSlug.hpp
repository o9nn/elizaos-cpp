#include ".services/defillama.service.hpp"
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



  // Parameter schema for tool calling

      // Read parameters from state (extracted by multiStepDecisionTemplate)

      // Extract and validate protocols parameter (required)

      // Parse comma-separated protocol names

      // Store input parameters for return

      // Search for multiple candidates per protocol name (0-5 matches each)
      using ProtocolSlugInfo = {

      using SearchResult = {

      // Try to capture input params even in failure


} // namespace elizaos
