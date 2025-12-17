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



void getExplorerUrl(const std::string& endpoint, const std::variant<std::string, PublicKey>& viewTypeOrItemAddress, auto itemType);
} // namespace elizaos
