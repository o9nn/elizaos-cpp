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
#include "global.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct RawHoldersQuery {
    std::optional<std::string> limit;
    std::optional<std::string> page;
};

struct ParsedHoldersQuery {
    std::string mint;
    double limit;
    double page;
    double offset;
};

ParsedHoldersQuery parseHoldersQuery(const std::string& mintParam, RawHoldersQuery query);

} // namespace elizaos
