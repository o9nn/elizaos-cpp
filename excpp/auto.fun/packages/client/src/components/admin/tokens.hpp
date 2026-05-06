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
#include "bonding-curve-bar.hpp"
#include "button.hpp"
#include "ui/table.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



 // Add fromNow and resizeImage

 // Import env

 // Import icons

 // Import BondingCurveBar

 // Import table components

using SortOrderType = std::string;

void AdminTokensList();

struct SocialLinks {
    std::string website;
    std::string twitter;
    std::string telegram;
    std::string discord;
    std::string farcaster;
};

void AdminTokenDetails();

} // namespace elizaos
