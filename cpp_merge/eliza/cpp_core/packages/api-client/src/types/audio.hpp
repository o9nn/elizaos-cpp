#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct SpeechConversationParams {
    std::variant<std::vector<uint8_t>, std:> audio;
    std::optional<std:> format;
    std::optional<std:> language;
};

struct SpeechGenerateParams {
    std: text;
    std::optional<std:> voice;
    std::optional<std:> language;
    std::optional<double> speed;
    std::optional<double> pitch;
};

struct AudioSynthesizeParams {
    UUID messageId;
    std::optional<std:> voice;
    std::optional<std:> format;
};

struct TranscribeParams {
    std::variant<std::vector<uint8_t>, std:> audio;
    std::optional<std:> format;
    std::optional<std:> language;
};

struct SpeechResponse {
    std::optional<std:> text;
    std::optional<std:> audio;
    std::optional<double> duration;
};

struct TranscriptionResponse {
    std: text;
    std::optional<std:> language;
    std::optional<double> confidence;
    std: word;
    double start;
    double end;
    std::optional<double> confidence;
};


} // namespace elizaos
