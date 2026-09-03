#pragma once

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
    int sample_rate = 0;
    int channels = 0;
    double duration_seconds = 0.0;
};

/**
 * Audio transcription result
 */
struct TranscriptionResult {
    std::string text;
    double confidence = 0.0;
    bool success = false;
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
    /**
     * Load audio file and return audio data
     */
    static AudioData loadAudioFile(const std::string& file_path);
    
    /**
     * Save audio data to file
     */
    static bool saveAudioFile(const AudioData& audio, const std::string& file_path);
    
    /**
     * Split audio into smaller chunks based on silence
     */
    static std::vector<AudioData> splitOnSilence(
        const AudioData& audio,
        int min_silence_len_ms = 1500,
        float silence_thresh_db = -60.0f,
        int keep_silence_ms = 250
    );
    
    /**
     * Filter audio chunks by duration
     */
    static std::vector<AudioData> filterByDuration(
        const std::vector<AudioData>& chunks,
        double min_duration = 1.0,
        double max_duration = 12.0
    );
    
    /**
     * Normalize audio levels
     */
    static AudioData normalize(const AudioData& audio);
    
    /**
     * Convert audio format/sample rate
     */
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
    
    /**
     * Transcribe audio data to text
     */
    TranscriptionResult transcribe(const AudioData& audio);
    
    /**
     * Transcribe audio file to text
     */
    TranscriptionResult transcribeFile(const std::string& file_path);
    
    /**
     * Batch transcribe multiple files
     */
    std::vector<std::pair<std::string, TranscriptionResult>> transcribeBatch(
        const std::vector<std::string>& file_paths
    );
    
    /**
     * Set transcription model/service
     */
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
    
    /**
     * Synthesize text to audio
     */
    AudioData synthesize(const std::string& text, const SynthesisConfig& config = {});
    
    /**
     * Synthesize text to audio file
     */
    bool synthesizeToFile(
        const std::string& text,
        const std::string& output_path,
        const SynthesisConfig& config = {}
    );
    
    /**
     * Set synthesis model/voice
     */
    void setSynthesisModel(const std::string& model_name);
    
    /**
     * List available voices
     */
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
    /**
     * LJSpeech metadata entry
     */
    struct MetadataEntry {
        std::string audio_file;
        std::string transcription;
        std::string normalized_transcription;
    };
    
    /**
     * Process audio files and create LJSpeech dataset
     */
    static std::vector<MetadataEntry> createDataset(
        const std::string& input_dir,
        const std::string& output_dir,
        bool split_long_audio = true,
        bool filter_short_audio = true
    );
    
    /**
     * Save metadata to CSV format
     */
    static bool saveMetadata(
        const std::vector<MetadataEntry>& metadata,
        const std::string& output_path
    );
    
    /**
     * Load metadata from CSV format
     */
    static std::vector<MetadataEntry> loadMetadata(const std::string& input_path);
    
    /**
     * Validate dataset integrity
     */
    static bool validateDataset(const std::string& dataset_dir);
    
    /**
     * Clean and normalize transcription text
     */
    static std::string normalizeTranscription(const std::string& text);
};

/**
 * Main LJSpeechTools interface
 */
class LJSpeechTools {
public:
    LJSpeechTools();
    ~LJSpeechTools();
    
    /**
     * Initialize the tools with configuration
     */
    bool initialize(const std::string& config_path = "");
    
    /**
     * Run the complete pipeline from audio files to dataset
     */
    bool runPipeline(
        const std::string& input_dir,
        const std::string& output_dir,
        bool verbose = true
    );
    
    /**
     * Get audio processor instance
     */
    AudioProcessor& getAudioProcessor();
    
    /**
     * Get speech transcriber instance
     */
    SpeechTranscriber& getTranscriber();
    
    /**
     * Get speech synthesizer instance
     */
    SpeechSynthesizer& getSynthesizer();
    
    /**
     * Get dataset preparator instance
     */
    DatasetPreparator& getDatasetPreparator();
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace elizaos