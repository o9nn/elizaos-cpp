#pragma once

/**
 * ElizaOS C++ - ljspeechtools Module
 *
 * Speech processing and LJSpeech dataset preparation tools.
 */

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace elizaos {

/** Audio samples are interleaved by channel. */
struct AudioData {
    std::vector<float> samples;
    int sample_rate = 0;
    int channels = 0;
    double duration_seconds = 0.0;
};

/** Time-bounded backend text using the half-open frame range [begin, end). */
struct TranscriptionSegment {
    std::size_t begin_frame = 0;
    std::size_t end_frame = 0;
    std::string text;
    double confidence = 0.0;
};

struct TranscriptionResult {
    std::string text;
    double confidence = 0.0;
    bool success = false;
    std::string error_message;
    std::vector<TranscriptionSegment> segments;
};

/** Validated, interleaved audio and selected model sent to a backend. */
struct TranscriptionRequest {
    AudioData audio;
    std::string model_name;
};

/** Explicit external transcription boundary; no backend is installed by default. */
class TranscriptionBackend {
public:
    virtual ~TranscriptionBackend() = default;
    virtual TranscriptionResult transcribe(const TranscriptionRequest& request) = 0;
};

struct SynthesisConfig {
    int sample_rate = 22050;
    int channels = 1;
    float speed = 1.0f;
    float pitch = 1.0f;
    std::string voice_model = "default";
};

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

class SpeechTranscriber {
public:
    explicit SpeechTranscriber(std::shared_ptr<TranscriptionBackend> backend = {});
    ~SpeechTranscriber();
    TranscriptionResult transcribe(const AudioData& audio);
    TranscriptionResult transcribeFile(const std::string& file_path);
    std::vector<std::pair<std::string, TranscriptionResult>> transcribeBatch(
        const std::vector<std::string>& file_paths
    );
    void setTranscriptionModel(const std::string& model_name);
    void setBackend(std::shared_ptr<TranscriptionBackend> backend);
    bool hasBackend() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

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

class DatasetPreparator {
public:
    struct MetadataEntry {
        std::string audio_file;
        std::string transcription;
        std::string normalized_transcription;
    };

    /** Uses a default transcriber, which yields no entries without a backend. */
    static std::vector<MetadataEntry> createDataset(
        const std::string& input_dir,
        const std::string& output_dir,
        bool split_long_audio = true,
        bool filter_short_audio = true
    );
    static std::vector<MetadataEntry> createDataset(
        const std::string& input_dir,
        const std::string& output_dir,
        SpeechTranscriber& transcriber,
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
