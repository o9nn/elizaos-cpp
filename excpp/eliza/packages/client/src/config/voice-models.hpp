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

struct VoiceModel {
    std::string value;
    std::string label;
    std::variant<'local', 'elevenlabs', 'openai', 'none'> provider;
    std::optional<std::variant<'male', 'female'>> gender;
    std::optional<std::string> language;
    std::optional<std::vector<std::string>> features;
};

// TODO: ELI2-218 Refactor this to use plugin categories when available
// This hardcoded mapping will be replaced with a more flexible approach
// that leverages plugin category metadata once implemented

// No voice option for agents that don't need speech capabilities

  // Filter out empty strings (for "none" provider)

} // namespace elizaos
