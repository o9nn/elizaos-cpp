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



// Define cookie types locally as CookieProvider will be removed
using CookieType = std::variant<"necessary", "analytics", "preferences">;


} // namespace elizaos
