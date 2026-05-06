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



// Re-BigNumber constructor

// Helper function to create new BigNumber instances
/**
 * Convert a string, number, or BigNumber to a BigNumber object.
 *
 * @param value - The value to convert to a BigNumber.
 * @returns A BigNumber object representing the input value.
 */

BigNumber toBN(const std::variant<std::string, double, BigNumber>& value);

} // namespace elizaos
