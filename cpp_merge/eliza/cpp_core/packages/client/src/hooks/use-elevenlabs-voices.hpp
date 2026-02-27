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
    std: voice_id;
    std: name;
    std: category;
    std::optional<std:> accent;
    std::optional<std:> age;
    std::optional<std:> description;
    std::optional<std:> gender;
    std::optional<std:> use_case;
    std::optional<std:> preview_url;
};

void useElevenLabsVoices();

} // namespace elizaos
