#include "elizaos/core-plugin-v2.hpp"
#include "state.hpp"
#include "types.hpp"
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
;
;

/**
 * Provider for external data/services
 * This is a v1 compatibility wrapper for v2 Provider
 */
using Provider = ProviderFromTypes;

/**
 * Converts v2 Provider to v1 compatible Provider
 * Uses the V2 Provider interface to ensure proper optional field handling
 */
 catch (error) {
        console.error(`Error in v2 provider ${providerV2.name}:`, error);
        throw error;
      }
    },
  };
}

/**
 * Converts v1 Provider to v2 Provider
 * Creates a Provider object conforming to V2 Provider interface
 */
,
            data: result.data || {},
            text: result.text || '',
          };
        }

        // For primitive results, return as text
        return {
          values: {},
          data: {},
          text: String(result || ''),
        };
      } catch (error) {
        console.error(`Error in v1 provider ${provider.name || 'unnamed'}:`, error);
        throw error;
      }
    },
  };
}

} // namespace elizaos
