#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;

struct SpeechConversationParams {
    Blob | Buffer | string audio;
    std::optional<'mp3' | 'wav' | 'ogg' | 'webm'> format;
    std::optional<std::string> language;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
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
    std::optional<'mp3' | 'wav' | 'ogg'> format;
};


struct TranscribeParams {
    Blob | Buffer | string audio;
    std::optional<'mp3' | 'wav' | 'ogg' | 'webm'> format;
    std::optional<std::string> language;
};


struct SpeechResponse {
    std::optional<std::string> text;
    std::optional<std::string> audio;
    std::optional<double> duration;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};


struct TranscriptionResponse {
    std::string text;
    std::optional<std::string> language;
    std::optional<double> confidence;
    std::optional<Array<{> words;
    std::string word;
    double start;
    double end;
    std::optional<double> confidence;
};


} // namespace elizaos
