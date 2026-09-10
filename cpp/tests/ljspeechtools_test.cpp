#include <gtest/gtest.h>
#include "elizaos/ljspeechtools.hpp"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <limits>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

using namespace elizaos;
namespace fs = std::filesystem;

namespace {

AudioData makeAudio(int sample_rate = 16000, int channels = 1,
                    std::size_t frames = 16000, float amplitude = 0.25f) {
    AudioData audio;
    audio.sample_rate = sample_rate;
    audio.channels = channels;
    audio.duration_seconds = static_cast<double>(frames) / sample_rate;
    audio.samples.resize(frames * static_cast<std::size_t>(channels));
    for (std::size_t frame = 0; frame < frames; ++frame) {
        const float value = amplitude * static_cast<float>(std::sin(
            2.0 * 3.14159265358979323846 * 440.0 *
            static_cast<double>(frame) / sample_rate));
        for (int channel = 0; channel < channels; ++channel) {
            audio.samples[frame * static_cast<std::size_t>(channels) +
                          static_cast<std::size_t>(channel)] =
                channel == 0 ? value : -value;
        }
    }
    return audio;
}

TranscriptionResult successfulResult(const std::string& text = "truthful transcript") {
    TranscriptionResult result;
    result.success = true;
    result.text = text;
    result.confidence = 0.875;
    return result;
}

class CapturingBackend final : public TranscriptionBackend {
public:
    explicit CapturingBackend(TranscriptionResult response = successfulResult())
        : response_(std::move(response)) {}

    TranscriptionResult transcribe(const TranscriptionRequest& request) override {
        std::lock_guard<std::mutex> lock(mutex_);
        requests_.push_back(request);
        return response_;
    }

    std::vector<TranscriptionRequest> requests() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return requests_;
    }

private:
    mutable std::mutex mutex_;
    std::vector<TranscriptionRequest> requests_;
    TranscriptionResult response_;
};

class SequenceBackend final : public TranscriptionBackend {
public:
    TranscriptionResult transcribe(const TranscriptionRequest& request) override {
        const int call = calls_.fetch_add(1);
        return successfulResult("clip " + std::to_string(call) + " " +
                                std::to_string(request.audio.samples.size()));
    }

private:
    std::atomic<int> calls_{0};
};

class ThrowingBackend final : public TranscriptionBackend {
public:
    TranscriptionResult transcribe(const TranscriptionRequest&) override {
        throw std::runtime_error("backend unavailable");
    }
};

class LJSpeechToolsTest : public ::testing::Test {
protected:
    void SetUp() override {
        static std::atomic<unsigned long> sequence{0};
        test_dir_ = fs::temp_directory_path() /
            ("ljspeech_test_" + std::to_string(sequence.fetch_add(1)));
        input_dir_ = test_dir_ / "input";
        output_dir_ = test_dir_ / "output";
        fs::create_directories(input_dir_);
        fs::create_directories(output_dir_);
        for (int index = 0; index < 3; ++index) {
            ASSERT_TRUE(AudioProcessor::saveAudioFile(
                makeAudio(44100, 1, 44100, 0.2f + 0.02f * index),
                (input_dir_ / ("test_audio_" + std::to_string(index) + ".wav")).string()));
        }
    }

    void TearDown() override {
        std::error_code error;
        fs::remove_all(test_dir_, error);
    }

    fs::path test_dir_;
    fs::path input_dir_;
    fs::path output_dir_;
};

TEST_F(LJSpeechToolsTest, PcmWaveRoundTripPreservesFramesAndChannels) {
    const AudioData original = makeAudio(22050, 2, 257);
    const fs::path path = output_dir_ / "stereo.wav";
    ASSERT_TRUE(AudioProcessor::saveAudioFile(original, path.string()));
    const AudioData loaded = AudioProcessor::loadAudioFile(path.string());
    EXPECT_EQ(loaded.sample_rate, 22050);
    EXPECT_EQ(loaded.channels, 2);
    ASSERT_EQ(loaded.samples.size(), original.samples.size());
    EXPECT_NEAR(loaded.duration_seconds, 257.0 / 22050.0, 1e-12);
    EXPECT_NEAR(loaded.samples[20], original.samples[20], 4e-5);
}

TEST_F(LJSpeechToolsTest, WaveReaderRejectsMissingTruncatedAndIncompleteFrames) {
    EXPECT_THROW(AudioProcessor::loadAudioFile((input_dir_ / "missing.wav").string()),
                 std::runtime_error);
    const fs::path malformed = input_dir_ / "malformed.wav";
    std::ofstream(malformed, std::ios::binary) << "RIFF\x24\0\0\0WAVEfmt ";
    EXPECT_THROW(AudioProcessor::loadAudioFile(malformed.string()), std::runtime_error);

    AudioData incomplete = makeAudio(16000, 2, 2);
    incomplete.samples.pop_back();
    EXPECT_FALSE(AudioProcessor::saveAudioFile(
        incomplete, (output_dir_ / "incomplete.wav").string()));
}

TEST_F(LJSpeechToolsTest, SplitAndConversionRemainChannelAware) {
    AudioData stereo;
    stereo.sample_rate = 10;
    stereo.channels = 2;
    stereo.samples = {
        0.5f, -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, -0.5f};
    stereo.duration_seconds = 0.8;

    const auto chunks = AudioProcessor::splitOnSilence(stereo, 300, -40.0f, 0);
    ASSERT_EQ(chunks.size(), 2U);
    EXPECT_EQ(chunks[0].samples.size() % 2U, 0U);
    EXPECT_EQ(chunks[1].samples.size() % 2U, 0U);

    const auto resampled = AudioProcessor::convertFormat(stereo, 20, 2);
    ASSERT_EQ(resampled.samples.size(), 32U);
    for (std::size_t frame = 0; frame < resampled.samples.size() / 2U; ++frame) {
        EXPECT_NEAR(resampled.samples[frame * 2U],
                    -resampled.samples[frame * 2U + 1U], 1e-6);
    }
    const auto mono = AudioProcessor::convertFormat(stereo, 10, 1);
    ASSERT_EQ(mono.samples.size(), 8U);
    for (float sample : mono.samples) {
        EXPECT_NEAR(sample, 0.0f, 1e-6);
    }
}

TEST_F(LJSpeechToolsTest, DefaultTranscriberRejectsWithoutFabricatingText) {
    SpeechTranscriber transcriber;
    EXPECT_FALSE(transcriber.hasBackend());
    const auto result = transcriber.transcribe(makeAudio());
    EXPECT_FALSE(result.success);
    EXPECT_TRUE(result.text.empty());
    EXPECT_DOUBLE_EQ(result.confidence, 0.0);
    EXPECT_NE(result.error_message.find("No transcription backend"), std::string::npos);
}

TEST_F(LJSpeechToolsTest, FakeBackendCapturesCanonicalInputAndModel) {
    auto backend = std::make_shared<CapturingBackend>();
    SpeechTranscriber transcriber(backend);
    transcriber.setTranscriptionModel("fixture-model-v1");
    AudioData audio = makeAudio(8000, 2, 80);
    audio.duration_seconds = 999.0;

    const auto result = transcriber.transcribe(audio);
    ASSERT_TRUE(result.success) << result.error_message;
    EXPECT_EQ(result.text, "truthful transcript");
    const auto requests = backend->requests();
    ASSERT_EQ(requests.size(), 1U);
    EXPECT_EQ(requests[0].model_name, "fixture-model-v1");
    EXPECT_EQ(requests[0].audio.samples, audio.samples);
    EXPECT_EQ(requests[0].audio.sample_rate, 8000);
    EXPECT_EQ(requests[0].audio.channels, 2);
    EXPECT_DOUBLE_EQ(requests[0].audio.duration_seconds, 0.01);
}

TEST_F(LJSpeechToolsTest, InvalidAudioAndModelAreRejectedBeforeBackendCall) {
    auto backend = std::make_shared<CapturingBackend>();
    SpeechTranscriber transcriber(backend);
    std::vector<AudioData> invalid;
    invalid.push_back(AudioData{});
    invalid.push_back(makeAudio(0, 1, 4));
    invalid.push_back(makeAudio(16000, 0, 4));
    invalid.push_back(makeAudio(16000, 2, 4));
    invalid.back().samples.pop_back();
    invalid.push_back(makeAudio(16000, 1, 4));
    invalid.back().samples[0] = std::numeric_limits<float>::quiet_NaN();
    invalid.push_back(makeAudio(16000, 1, 4));
    invalid.back().samples[0] = 1.1f;
    invalid.push_back(makeAudio(16000, 1, 4));
    invalid.back().duration_seconds = -1.0;

    for (const auto& audio : invalid) {
        const auto result = transcriber.transcribe(audio);
        EXPECT_FALSE(result.success);
        EXPECT_TRUE(result.text.empty());
        EXPECT_FALSE(result.error_message.empty());
    }
    transcriber.setTranscriptionModel(" \t");
    EXPECT_FALSE(transcriber.transcribe(makeAudio()).success);
    EXPECT_TRUE(backend->requests().empty());
}

TEST_F(LJSpeechToolsTest, BackendFailureAndExceptionsPropagateAsFailures) {
    TranscriptionResult failure;
    failure.error_message = "decoder refused input";
    auto failing = std::make_shared<CapturingBackend>(failure);
    SpeechTranscriber transcriber(failing);
    auto result = transcriber.transcribe(makeAudio());
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.error_message, "decoder refused input");
    EXPECT_TRUE(result.text.empty());

    transcriber.setBackend(std::make_shared<ThrowingBackend>());
    result = transcriber.transcribe(makeAudio());
    EXPECT_FALSE(result.success);
    EXPECT_NE(result.error_message.find("backend unavailable"), std::string::npos);
    EXPECT_TRUE(result.text.empty());
}

TEST_F(LJSpeechToolsTest, MalformedBackendSuccessIsRejected) {
    std::vector<TranscriptionResult> malformed;
    auto empty = successfulResult("");
    malformed.push_back(empty);
    auto invalid_confidence = successfulResult();
    invalid_confidence.confidence = 2.0;
    malformed.push_back(invalid_confidence);
    auto contradictory = successfulResult();
    contradictory.error_message = "also failed";
    malformed.push_back(contradictory);
    auto out_of_bounds = successfulResult();
    out_of_bounds.segments = {{0, 20000, "too long", 0.9}};
    malformed.push_back(out_of_bounds);
    auto unordered = successfulResult();
    unordered.text.clear();
    unordered.segments = {{100, 200, "second", 0.9}, {50, 80, "first", 0.9}};
    malformed.push_back(unordered);
    auto mismatch = successfulResult("different");
    mismatch.segments = {{0, 10, "segment", 0.9}};
    malformed.push_back(mismatch);

    for (const auto& response : malformed) {
        SpeechTranscriber transcriber(std::make_shared<CapturingBackend>(response));
        const auto result = transcriber.transcribe(makeAudio());
        EXPECT_FALSE(result.success);
        EXPECT_TRUE(result.text.empty());
        EXPECT_NE(result.error_message.find("Malformed"), std::string::npos);
    }
}

TEST_F(LJSpeechToolsTest, OrderedSegmentsAreJoinedAndAdjacentDuplicatesDeduplicated) {
    TranscriptionResult response;
    response.success = true;
    response.confidence = 0.8;
    response.segments = {
        {0, 100, "hello", 0.9},
        {100, 200, "hello", 0.8},
        {200, 300, "world", 0.7}};
    SpeechTranscriber transcriber(std::make_shared<CapturingBackend>(response));
    const auto result = transcriber.transcribe(makeAudio());
    ASSERT_TRUE(result.success) << result.error_message;
    EXPECT_EQ(result.text, "hello world");
    ASSERT_EQ(result.segments.size(), 2U);
    EXPECT_EQ(result.segments[0].begin_frame, 0U);
    EXPECT_EQ(result.segments[0].end_frame, 200U);
    EXPECT_DOUBLE_EQ(result.segments[0].confidence, 0.8);
}

TEST_F(LJSpeechToolsTest, FileAndBatchTranscriptionPreserveInputOrder) {
    auto backend = std::make_shared<SequenceBackend>();
    SpeechTranscriber transcriber(backend);
    const std::vector<std::string> files = {
        (input_dir_ / "test_audio_2.wav").string(),
        (input_dir_ / "missing.wav").string(),
        (input_dir_ / "test_audio_0.wav").string()};
    const auto results = transcriber.transcribeBatch(files);
    ASSERT_EQ(results.size(), files.size());
    EXPECT_EQ(results[0].first, files[0]);
    EXPECT_TRUE(results[0].second.success);
    EXPECT_EQ(results[1].first, files[1]);
    EXPECT_FALSE(results[1].second.success);
    EXPECT_EQ(results[2].first, files[2]);
    EXPECT_TRUE(results[2].second.success);
    EXPECT_EQ(results[0].second.text, "clip 0 44100");
    EXPECT_EQ(results[2].second.text, "clip 1 44100");
}

TEST_F(LJSpeechToolsTest, ConcurrentSynchronousCallsAndTeardownAreSafe) {
    for (int iteration = 0; iteration < 20; ++iteration) {
        auto backend = std::make_shared<CapturingBackend>();
        auto transcriber = std::make_unique<SpeechTranscriber>(backend);
        std::vector<std::thread> workers;
        for (int worker = 0; worker < 8; ++worker) {
            workers.emplace_back([&transcriber] {
                for (int call = 0; call < 10; ++call) {
                    EXPECT_TRUE(transcriber->transcribe(makeAudio(8000, 1, 16)).success);
                }
            });
        }
        for (auto& worker : workers) {
            worker.join();
        }
        EXPECT_EQ(backend->requests().size(), 80U);
        transcriber.reset();
    }
}

TEST_F(LJSpeechToolsTest, DatasetRequiresBackendAndLeavesNoOrphanAudioOnFailure) {
    const auto metadata = DatasetPreparator::createDataset(
        input_dir_.string(), output_dir_.string(), true, true);
    EXPECT_TRUE(metadata.empty());
    EXPECT_TRUE(fs::is_directory(output_dir_ / "wavs"));
    EXPECT_TRUE(fs::is_empty(output_dir_ / "wavs"));
}

TEST_F(LJSpeechToolsTest, DatasetWithFakeBackendCommitsAudioAndTruthfulMetadata) {
    auto backend = std::make_shared<CapturingBackend>(successfulResult("Known TEXT!"));
    SpeechTranscriber transcriber(backend);
    const auto metadata = DatasetPreparator::createDataset(
        input_dir_.string(), output_dir_.string(), transcriber, true, true);
    ASSERT_EQ(metadata.size(), 3U);
    for (std::size_t index = 0; index < metadata.size(); ++index) {
        EXPECT_EQ(metadata[index].audio_file,
                  "wavs/sample_" + std::to_string(index) + "_0.wav");
        EXPECT_EQ(metadata[index].transcription, "Known TEXT!");
        EXPECT_EQ(metadata[index].normalized_transcription, "known text");
        EXPECT_TRUE(fs::is_regular_file(output_dir_ / metadata[index].audio_file));
    }
    ASSERT_TRUE(DatasetPreparator::saveMetadata(
        metadata, (output_dir_ / "metadata.csv").string()));
    EXPECT_TRUE(DatasetPreparator::validateDataset(output_dir_.string()));
}

TEST_F(LJSpeechToolsTest, MetadataSaveRejectsMalformedRowsWithoutReplacingExistingFile) {
    const fs::path metadata_path = output_dir_ / "metadata.csv";
    std::ofstream(metadata_path) << "original\n";
    DatasetPreparator::MetadataEntry malformed{
        "wavs/a.wav", "contains|delimiter", "contains delimiter"};
    EXPECT_FALSE(DatasetPreparator::saveMetadata({malformed}, metadata_path.string()));
    std::ifstream input(metadata_path);
    std::string content;
    std::getline(input, content);
    EXPECT_EQ(content, "original");
    EXPECT_FALSE(fs::exists(metadata_path.string() + ".tmp"));
}

TEST_F(LJSpeechToolsTest, DatasetValidationRequiresReferencedValidWaveFiles) {
    fs::create_directories(output_dir_ / "wavs");
    std::ofstream(output_dir_ / "metadata.csv")
        << "wavs/missing.wav|real text|real text\n";
    EXPECT_FALSE(DatasetPreparator::validateDataset(output_dir_.string()));
}

TEST_F(LJSpeechToolsTest, PipelineFailsWithoutBackendAndSucceedsWithExplicitFake) {
    LJSpeechTools tools;
    EXPECT_TRUE(tools.initialize());
    EXPECT_FALSE(tools.runPipeline(input_dir_.string(), output_dir_.string(), false));
    EXPECT_FALSE(fs::exists(output_dir_ / "metadata.csv"));

    tools.getTranscriber().setBackend(std::make_shared<CapturingBackend>());
    EXPECT_TRUE(tools.runPipeline(input_dir_.string(), output_dir_.string(), false));
    EXPECT_TRUE(DatasetPreparator::validateDataset(output_dir_.string()));
}

TEST_F(LJSpeechToolsTest, SynthesisConfigurationBoundsAreValidated) {
    SpeechSynthesizer synthesizer;
    SynthesisConfig config;
    config.sample_rate = 0;
    EXPECT_TRUE(synthesizer.synthesize("text", config).samples.empty());
    config.sample_rate = 22050;
    config.channels = 0;
    EXPECT_TRUE(synthesizer.synthesize("text", config).samples.empty());
    config.channels = 1;
    config.speed = 0.0f;
    EXPECT_TRUE(synthesizer.synthesize("text", config).samples.empty());
    config.speed = 1.0f;
    config.pitch = std::numeric_limits<float>::infinity();
    EXPECT_TRUE(synthesizer.synthesize("text", config).samples.empty());
}

TEST_F(LJSpeechToolsTest, NormalizationAndDurationFilteringAreDeterministic) {
    EXPECT_EQ(DatasetPreparator::normalizeTranscription(
                  "Dr. Smith can't visit 2nd Ave.!"),
              "doctor smith cannot visit second avenue");
    EXPECT_TRUE(DatasetPreparator::normalizeTranscription(" \t\n").empty());

    std::vector<AudioData> chunks(3);
    chunks[0].duration_seconds = 0.5;
    chunks[1].duration_seconds = 2.0;
    chunks[2].duration_seconds = 13.0;
    const auto filtered = AudioProcessor::filterByDuration(chunks, 1.0, 12.0);
    ASSERT_EQ(filtered.size(), 1U);
    EXPECT_DOUBLE_EQ(filtered[0].duration_seconds, 2.0);
}

}  // namespace
