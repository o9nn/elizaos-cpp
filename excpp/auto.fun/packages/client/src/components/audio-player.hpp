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
#include "copy-button.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct AudioPlayerProps {
    std::string src;
    std::optional<std::string> albumArt;
    std::optional<std::string> lyrics;
    std::optional<bool> showControls;
    std::optional<std::string> className;
};


} // namespace elizaos
