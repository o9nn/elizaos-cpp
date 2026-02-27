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
    std: title;
    std: description;
    std: preview;
    std::vector<std::string> tags;
    std::optional<std:> twitter;
    std::optional<std:> discord;
    std::optional<std:> telegram;
    std: slug;
};

// Memoized partner card component to prevent unnecessary re-renders


} // namespace elizaos
