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
#include "Socials.module.css.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct SocialButtonProps {
    std::string provider;
    std::optional<bool> isActive;
};

  // Check if this provider is the active one

  // Commented out GitHub and Twitter, only using Discord for now


} // namespace elizaos
