#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct SpeechConversationParams {
    std::variant<Blob, Buffer, std::string> audio;
    std::optional<std::string> format;
    std::optional<std::string> language;
};

struct SpeechGenerateParams {
    std::string text;
    std::optional<std::string> voice;
    std::optional<std::string> language;
    std::optional<double> speed;
    std::optional<double> pitch;
};

struct AudioSynthesizeParams {
    UUID messageId;
    std::optional<std::string> voice;
    std::optional<std::string> format;
};

struct TranscribeParams {
    std::variant<Blob, Buffer, std::string> audio;
    std::optional<std::string> format;
    std::optional<std::string> language;
};

struct SpeechResponse {
    std::optional<std::string> text;
    std::optional<std::string> audio;
    std::optional<double> duration;
};

struct TranscriptionResponse {
    std::string text;
    std::optional<std::string> language;
    std::optional<double> confidence;
    std::string word;
    double start;
    double end;
    std::optional<double> confidence;
};


} // namespace elizaos
