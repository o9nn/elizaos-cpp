#pragma once

/**
 * ElizaOS C++ - ljspeechtools Module
 *
 * Speech processing and LJSpeech dataset preparation tools.
 * Full implementation is in cpp/external/ljspeechtools/src/ljspeechtools.cpp
 * with the canonical header at include/elizaos/ljspeechtools.hpp.
 *
 * This header mirrors the canonical declarations so that both include paths
 * resolve consistently regardless of search-path ordering.
 */

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <cstdint>

namespace elizaos {

/**
 * Audio data structure for processing
 */
struct AudioData {
    std::vector<float> samples;
    int sample_rate;
    int channels;
    double duration_seconds;
};

/**
 * Audio transcription result
 */
struct TranscriptionResult {
    std::string text;
    double confidence;
    bool success;
    std::string error_message;
};

/**
 * Audio synthesis configuration
 */
struct SynthesisConfig {
    int sample_rate = 22050;
    int channels = 1;
    float speed = 1.0f;
    float pitch = 1.0f;
    std::string voice_model = "default";
};

/**
 * Audio processing utilities for LJSpeech dataset preparation
 */
class AudioProcessor {
public:
    static AudioData loadAudioFile(const std::string& file_path);
    static bool saveAudioFile(const AudioData& audio, const std::string& file_path);
    static std::vector<AudioData> splitOnSilence(
        const AudioData& audio,
        int min_silence_len_ms = 1500,
        float silence_thresh_db = -60.0f,
        int keep_silence_ms = 250
    );
    static std::vector<AudioData> filterByDuration(
        const std::vector<AudioData>& chunks,
        double min_duration = 1.0,
        double max_duration = 12.0
    );
    static AudioData normalize(const AudioData& audio);
    static AudioData convertFormat(
        const AudioData& audio,
        int target_sample_rate,
        int target_channels
    );
};

/**
 * Speech transcription service
 */
class SpeechTranscriber {
public:
    SpeechTranscriber();
    ~SpeechTranscriber();
    TranscriptionResult transcribe(const AudioData& audio);
    TranscriptionResult transcribeFile(const std::string& file_path);
    std::vector<std::pair<std::string, TranscriptionResult>> transcribeBatch(
        const std::vector<std::string>& file_paths
    );
    void setTranscriptionModel(const std::string& model_name);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

/**
 * Speech synthesis service
 */
class SpeechSynthesizer {
public:
    SpeechSynthesizer();
    ~SpeechSynthesizer();
    AudioData synthesize(const std::string& text, const SynthesisConfig& config = {});
    bool synthesizeToFile(
        const std::string& text,
        const std::string& output_path,
        const SynthesisConfig& config = {}
    );
    void setSynthesisModel(const std::string& model_name);
    std::vector<std::string> getAvailableVoices();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

/**
 * Dataset preparation utilities for LJSpeech format
 */
class DatasetPreparator {
public:
    struct MetadataEntry {
        std::string audio_file;
        std::string transcription;
        std::string normalized_transcription;
    };

    static std::vector<MetadataEntry> createDataset(
        const std::string& input_dir,
        const std::string& output_dir,
        bool split_long_audio = true,
        bool filter_short_audio = true
    );
    static bool saveMetadata(
        const std::vector<MetadataEntry>& metadata,
        const std::string& output_path
    );
    static std::vector<MetadataEntry> loadMetadata(const std::string& input_path);
    static bool validateDataset(const std::string& dataset_dir);
    static std::string normalizeTranscription(const std::string& text);
};

/**
 * Main LJSpeechTools interface
 */
class LJSpeechTools {
public:
    LJSpeechTools();
    ~LJSpeechTools();
    bool initialize(const std::string& config_path = "");
    bool runPipeline(
        const std::string& input_dir,
        const std::string& output_dir,
        bool verbose = true
    );
    AudioProcessor& getAudioProcessor();
    SpeechTranscriber& getTranscriber();
    SpeechSynthesizer& getSynthesizer();
    DatasetPreparator& getDatasetPreparator();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace elizaos
