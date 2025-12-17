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



// Configure BigNumber settings

/**
 * Creates a new BigNumber instance with proper error handling
 */
BigNumber toBN(const std::variant<std::string, double, BigNumber>& value);

/**
 * Formats a BigNumber to a human readable string
 */
std::string formatBN(BigNumber value, auto decimals = 18);


} // namespace elizaos
