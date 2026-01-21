#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Represents a type that can be either a NodePgDatabase or a PgliteDatabase.
 */
using DrizzleDatabase = std::variant<NodePgDatabase, PgliteDatabase>;

/**
 * Interface for managing a database client.
 * @template T - The type of the database connection object.
 */

} // namespace elizaos
