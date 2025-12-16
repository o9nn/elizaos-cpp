#include "service.hpp"
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



struct TranscriptionOptions {
    std::optional<std::string> language;
    std::optional<std::string> model;
    std::optional<double> temperature;
    std::optional<std::string> prompt;
    std::optional<'json' | 'text' | 'srt' | 'vtt' | 'verbose_json'> response_format;
    std::optional<std::vector<('word' | 'segment')>> timestamp_granularities;
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
    std::optional<'mp3' | 'wav' | 'flac' | 'aac'> format;
    std::optional<'mp3' | 'opus' | 'aac' | 'flac'> response_format;
};

/**
 * Interface for audio transcription and speech services
 */

  /**
   * Transcribe audio file to text
   * @param audioPath - Path to audio file or audio buffer
   * @param options - Transcription options
   * @returns Promise resolving to transcription result
   */

  /**
   * Transcribe video file to text (extracts audio first)
   * @param videoPath - Path to video file or video buffer
   * @param options - Transcription options
   * @returns Promise resolving to transcription result
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

  /**
   * Detect language of audio file
   * @param audioPath - Path to audio file or audio buffer
   * @returns Promise resolving to detected language code
   */

} // namespace elizaos
