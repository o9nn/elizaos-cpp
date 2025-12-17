#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Interface for defining the structure of an API response.
 * @template T - The type of data included in the response.
 */

/**
 * Defines the structure of AgentBasic interface.
 */
struct AgentBasic {
    std::string id;
    std::string name;
    std::optional<std::string> status;
};


} // namespace elizaos
