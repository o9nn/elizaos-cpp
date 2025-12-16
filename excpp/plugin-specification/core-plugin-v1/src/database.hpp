#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core-plugin-v2.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Database adapter interface for v1 compatibility
 */
using IDatabaseAdapter = IDatabaseAdapterFromTypes;

/**
 * Converts V2 Entity to V1 Account
 * V2 uses Entity with names array, V1 uses Account with single name/username
 */
Account fromV2Entity(Entity entity);

/**
 * Converts V1 Account to V2 Entity
 * V1 uses Account with single name/username, V2 uses Entity with names array
 */
Entity toV2Entity(Account account);

/**
 * Creates a wrapper that converts V2 IDatabaseAdapter to V1 compatible interface
 * This handles the major API differences between v1 and v2
 */
IDatabaseAdapter fromV2DatabaseAdapter(IDatabaseAdapterV2 adapterV2);

/**
 * Creates a wrapper that converts V1 IDatabaseAdapter to V2 compatible interface
 * This is a more complex conversion due to the significant API differences
 */
IDatabaseAdapterV2 toV2DatabaseAdapter(IDatabaseAdapter adapterV1);
} // namespace elizaos
