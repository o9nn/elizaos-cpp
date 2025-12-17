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



struct MediaContentProps {
    std::string url;
    std::optional<std::string> title;
    std::optional<std::string> className;
    std::optional<double> maxWidth;
    std::optional<double> maxHeight;
};

// Utility functions to detect media types


} // namespace elizaos
