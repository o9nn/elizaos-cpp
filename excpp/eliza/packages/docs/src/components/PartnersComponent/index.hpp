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
#include "styles.module.css.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Define the PartnerType interface
struct PartnerType {
    std::string title;
    std::string description;
    std::string preview;
    std::vector<std::string> tags;
    std::optional<std::string> twitter;
    std::optional<std::string> discord;
    std::optional<std::string> telegram;
    std::string slug;
};

// Memoized partner card component to prevent unnecessary re-renders


} // namespace elizaos
