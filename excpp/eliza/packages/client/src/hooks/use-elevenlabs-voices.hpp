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



// TODO: Move this to a shared config file, or the 11labs plugin once plugin categories are implemented

struct ElevenLabsVoice {
    std::string voice_id;
    std::string name;
    std::string category;
    std::optional<std::string> accent;
    std::optional<std::string> age;
    std::optional<std::string> description;
    std::optional<std::string> gender;
    std::optional<std::string> use_case;
    std::optional<std::string> preview_url;
};

void useElevenLabsVoices();

} // namespace elizaos
