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

;

// Configure BigNumber settings
BigNumber.config({
  DECIMAL_PLACES: 18,
  ROUNDING_MODE: BigNumber.ROUND_DOWN,
  EXPONENTIAL_AT: [-20, 20],
});

/**
 * Creates a new BigNumber instance with proper error handling
 */
 catch (error) {
    throw new Error(`Failed to convert value to BigNumber: ${value}`);
  }
}

/**
 * Formats a BigNumber to a human readable string
 */
 catch (error) {
    throw new Error(`Failed to format BigNumber: ${value}`);
  }
}

{ BigNumber as BN };

} // namespace elizaos
