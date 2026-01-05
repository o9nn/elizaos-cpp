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

// this just imported dotenv, settings will handle wrapping this

#include "messages.hpp"
//#include "models.hpp"
#include "posts.hpp"
//#include "providers.hpp"
//#include "relationships.hpp"
#include "runtime.hpp"

/*
*/

// Core types
#include "types.hpp"

// Adapters created for v1 -> v2 compatibility
// Export only the adapter functions and V1 types to avoid conflicts

// TODO: Implement the remaining adapters:
// - action/handler
// - database
// - knowledge / memory
// - relationships

} // namespace elizaos
