#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * CDP Plugin Type Definitions
 */

/**
 * Networks supported by CDP corresponding to DEFAULT_RPC_URLS
 */
using CdpNetwork = std::variant<, "ethereum", "base", "optimism", "arbitrum", "polygon", "base-sepolia">;


} // namespace elizaos
