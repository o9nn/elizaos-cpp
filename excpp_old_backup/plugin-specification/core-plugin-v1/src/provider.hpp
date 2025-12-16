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



/**
 * Provider for external data/services
 * This is a v1 compatibility wrapper for v2 Provider
 */
using Provider = ProviderFromTypes;

/**
 * Converts v2 Provider to v1 compatible Provider
 * Uses the V2 Provider interface to ensure proper optional field handling
 */
Provider fromV2Provider(ProviderV2 providerV2);

/**
 * Converts v1 Provider to v2 Provider
 * Creates a Provider object conforming to V2 Provider interface
 */
ProviderV2 toV2Provider(Provider provider);

} // namespace elizaos
