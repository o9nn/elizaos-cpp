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

struct VoiceModel {
    std: value;
    std: label;
    std: provider;
    std::optional<std:> gender;
    std::optional<std:> language;
    std::optional<std::vector<std::string>> features;
};

// TODO: ELI2-218 Refactor this to use plugin categories when available
// This hardcoded mapping will be replaced with a more flexible approach
// that leverages plugin category metadata once implemented

// No voice option for agents that don't need speech capabilities

  // Filter out empty strings (for "none" provider)

} // namespace elizaos
