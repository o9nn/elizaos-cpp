#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "service.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct TranscriptionOptions {
    std::optional<std::string> language;
    std::optional<std::string> model;
    std::optional<double> temperature;
    std::optional<std::string> prompt;
    std::optional<std::variant<'json', 'text', 'srt', 'vtt', 'verbose_json'>> response_format;
    std::optional<bool> word_timestamps;
    std::optional<bool> segment_timestamps;
};

struct TranscriptionResult {
    std::string text;
    std::optional<std::string> language;
    std::optional<double> duration;
    std::optional<std::vector<TranscriptionSegment>> segments;
    std::optional<std::vector<TranscriptionWord>> words;
    std::optional<double> confidence;
};

struct TranscriptionSegment {
    double id;
    std::string text;
    double start;
    double end;
    std::optional<double> confidence;
    std::optional<std::vector<double>> tokens;
    std::optional<double> temperature;
    std::optional<double> avg_logprob;
    std::optional<double> compression_ratio;
    std::optional<double> no_speech_prob;
};

struct TranscriptionWord {
    std::string word;
    double start;
    double end;
    std::optional<double> confidence;
};

struct SpeechToTextOptions {
    std::optional<std::string> language;
    std::optional<std::string> model;
    std::optional<bool> continuous;
    std::optional<bool> interimResults;
    std::optional<double> maxAlternatives;
};

struct TextToSpeechOptions {
    std::optional<std::string> voice;
    std::optional<std::string> model;
    std::optional<double> speed;
    std::optional<std::variant<'mp3', 'wav', 'flac', 'aac'>> format;
    std::optional<std::variant<'mp3', 'opus', 'aac', 'flac'>> response_format;
};

/**
 * Interface for audio transcription and speech services
 */

  /**
   * Real-time speech to text from audio stream
   * @param audioStream - Audio stream or buffer
   * @param options - Speech to text options
   * @returns Promise resolving to transcription result
   */

  /**
   * Convert text to speech
   * @param text - Text to convert to speech
   * @param options - Text to speech options
   * @returns Promise resolving to audio buffer
   */

  /**
   * Get supported languages for transcription
   * @returns Promise resolving to array of supported language codes
   */

  /**
   * Get available voices for text to speech
   * @returns Promise resolving to array of available voices
   */


} // namespace elizaos
