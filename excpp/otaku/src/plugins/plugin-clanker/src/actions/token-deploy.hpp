#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "services/clanker.service.hpp"
#include "types.hpp"
#include "utils/entity.hpp"
#include "utils/errors.hpp"
#include "utils/format.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Utility function to safely serialize objects with BigInt values
std::any safeStringify(const std::any& obj);

      // Check if services are available

      // Get services

      // Read parameters from state (extracted by multiStepDecisionTemplate)

      // Store input parameters for return

      // Validate required parameters

      // Prepare deploy parameters with proper structure

      // Validate parameters

      // Prepare response

      // Try to capture input params even in failure
        // If we can't get params, just use empty object


} // namespace elizaos
