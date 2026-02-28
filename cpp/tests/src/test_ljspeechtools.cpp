#include <gtest/gtest.h>
#include "elizaos/ljspeechtools.hpp"
#include "elizaos/agentlogger.hpp"
#include <filesystem>
#include <fstream>

using namespace elizaos;
namespace fs = std::filesystem;

class LJSpeechToolsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary directories for testing
        test_dir_ = fs::temp_directory_path() / "ljspeech_test";
        input_dir_ = test_dir_ / "input";
        output_dir_ = test_dir_ / "output";
        
        fs::create_directories(input_dir_);
        fs::create_directories(output_dir_);
        
        // Create mock audio files
        createMockAudioFiles();
    }
    
    void TearDown() override {
        // Clean up test directories
        if (fs::exists(test_dir_)) {
            fs::remove_all(test_dir_);
        }
    }
    
    void createMockAudioFiles() {
        // Create some mock audio files for testing
        for (int i = 0; i < 3; ++i) {
            std::string filename = "test_audio_" + std::to_string(i) + ".wav";
            std::string filepath = input_dir_ / filename;
            std::ofstream file(filepath, std::ios::binary);
            
            // Write minimal WAV header and data
            file << "RIFF    WAVEfmt ";
            file.close();
        }
    }
    
    fs::path test_dir_;
    fs::path input_dir_;
    fs::path output_dir_;
};

TEST_F(LJSpeechToolsTest, InitializationTest) {
    LJSpeechTools tools;
    EXPECT_TRUE(tools.initialize());
}

TEST_F(LJSpeechToolsTest, AudioProcessorLoadTest) {
    AudioData audio = AudioProcessor::loadAudioFile(input_dir_ / "test_audio_0.wav");
    
    EXPECT_EQ(audio.sample_rate, 44100);
    EXPECT_EQ(audio.channels, 1);
    EXPECT_GT(audio.samples.size(), 0);
    EXPECT_DOUBLE_EQ(audio.duration_seconds, 1.0);
}

TEST_F(LJSpeechToolsTest, AudioProcessorSaveTest) {
    AudioData audio;
    audio.samples.resize(1000, 0.5f);
    audio.sample_rate = 44100;
    audio.channels = 1;
    audio.duration_seconds = 1.0;
    
    std::string output_path = output_dir_ / "output_audio.wav";
    EXPECT_TRUE(AudioProcessor::saveAudioFile(audio, output_path));
}

TEST_F(LJSpeechToolsTest, AudioProcessorSplitTest) {
    AudioData audio;
    audio.samples.resize(44100 * 10, 0.5f); // 10 seconds of audio
    audio.sample_rate = 44100;
    audio.channels = 1;
    audio.duration_seconds = 10.0;
    
    auto chunks = AudioProcessor::splitOnSilence(audio);
    EXPECT_GT(chunks.size(), 0);
    
    for (const auto& chunk : chunks) {
        EXPECT_EQ(chunk.sample_rate, 44100);
        EXPECT_EQ(chunk.channels, 1);
        EXPECT_GT(chunk.samples.size(), 0);
    }
}

TEST_F(LJSpeechToolsTest, AudioProcessorFilterTest) {
    std::vector<AudioData> chunks;
    
    // Create chunks of various durations
    for (int i = 0; i < 5; ++i) {
        AudioData chunk;
        chunk.duration_seconds = i + 0.5; // 0.5s, 1.5s, 2.5s, 3.5s, 4.5s
        chunk.sample_rate = 44100;
        chunk.channels = 1;
        chunk.samples.resize(static_cast<size_t>(chunk.duration_seconds * chunk.sample_rate));
        chunks.push_back(chunk);
    }
    
    auto filtered = AudioProcessor::filterByDuration(chunks, 1.0, 4.0);
    EXPECT_EQ(filtered.size(), 3); // 1.5s, 2.5s, 3.5s should pass
}

TEST_F(LJSpeechToolsTest, AudioProcessorNormalizeTest) {
    AudioData audio;
    audio.samples = {0.5f, -0.8f, 1.2f, -1.5f}; // Some values outside [-1, 1]
    audio.sample_rate = 44100;
    audio.channels = 1;
    audio.duration_seconds = 4.0 / 44100.0;
    
    auto normalized = AudioProcessor::normalize(audio);
    
    // Check that all values are within [-0.9, 0.9]
    for (float sample : normalized.samples) {
        EXPECT_GE(sample, -0.9f);
        EXPECT_LE(sample, 0.9f);
    }
}

TEST_F(LJSpeechToolsTest, AudioProcessorConvertFormatTest) {
    AudioData audio;
    audio.samples.resize(1000, 0.5f);
    audio.sample_rate = 44100;
    audio.channels = 1;
    audio.duration_seconds = 1.0;
    
    auto converted = AudioProcessor::convertFormat(audio, 22050, 2);
    
    EXPECT_EQ(converted.sample_rate, 22050);
    EXPECT_EQ(converted.channels, 2);
}

TEST_F(LJSpeechToolsTest, SpeechTranscriberTest) {
    SpeechTranscriber transcriber;
    
    // Test transcribing audio data
    AudioData audio;
    audio.samples.resize(1000, 0.5f);
    audio.sample_rate = 44100;
    audio.channels = 1;
    audio.duration_seconds = 1.0;
    
    auto result = transcriber.transcribe(audio);
    
    EXPECT_TRUE(result.success);
    EXPECT_GT(result.text.length(), 0);
    EXPECT_GT(result.confidence, 0.0);
}

TEST_F(LJSpeechToolsTest, SpeechTranscriberFileTest) {
    SpeechTranscriber transcriber;
    
    std::string test_file = input_dir_ / "test_audio_0.wav";
    auto result = transcriber.transcribeFile(test_file);
    
    EXPECT_TRUE(result.success);
    EXPECT_GT(result.text.length(), 0);
    EXPECT_GT(result.confidence, 0.0);
}

TEST_F(LJSpeechToolsTest, SpeechTranscriberBatchTest) {
    SpeechTranscriber transcriber;
    
    std::vector<std::string> files = {
        input_dir_ / "test_audio_0.wav",
        input_dir_ / "test_audio_1.wav",
        input_dir_ / "test_audio_2.wav"
    };
    
    auto results = transcriber.transcribeBatch(files);
    
    EXPECT_EQ(results.size(), 3);
    
    for (const auto& [file, result] : results) {
        EXPECT_TRUE(result.success);
        EXPECT_GT(result.text.length(), 0);
    }
}

TEST_F(LJSpeechToolsTest, SpeechSynthesizerTest) {
    SpeechSynthesizer synthesizer;
    
    std::string text = "Hello, this is a test of speech synthesis.";
    auto audio = synthesizer.synthesize(text);
    
    EXPECT_GT(audio.samples.size(), 0);
    EXPECT_EQ(audio.sample_rate, 22050);
    EXPECT_EQ(audio.channels, 1);
    EXPECT_GT(audio.duration_seconds, 0.0);
}

TEST_F(LJSpeechToolsTest, SpeechSynthesizerConfigTest) {
    SpeechSynthesizer synthesizer;
    
    SynthesisConfig config;
    config.sample_rate = 48000;
    config.channels = 2;
    config.speed = 1.5f;
    config.pitch = 1.2f;
    
    std::string text = "Test with custom configuration.";
    auto audio = synthesizer.synthesize(text, config);
    
    EXPECT_GT(audio.samples.size(), 0);
    EXPECT_EQ(audio.sample_rate, 48000);
    EXPECT_EQ(audio.channels, 2);
}

TEST_F(LJSpeechToolsTest, SpeechSynthesizerToFileTest) {
    SpeechSynthesizer synthesizer;
    
    std::string text = "This will be saved to a file.";
    std::string output_file = output_dir_ / "synthesis_output.wav";
    
    EXPECT_TRUE(synthesizer.synthesizeToFile(text, output_file));
}

TEST_F(LJSpeechToolsTest, SpeechSynthesizerVoicesTest) {
    SpeechSynthesizer synthesizer;
    
    auto voices = synthesizer.getAvailableVoices();
    EXPECT_GT(voices.size(), 0);
    
    // Check that default voice is available
    EXPECT_NE(std::find(voices.begin(), voices.end(), "default"), voices.end());
}

TEST_F(LJSpeechToolsTest, DatasetPreparatorCreateTest) {
    auto metadata = DatasetPreparator::createDataset(
        input_dir_.string(),
        output_dir_.string(),
        true,
        true
    );
    
    EXPECT_GT(metadata.size(), 0);
    
    for (const auto& entry : metadata) {
        EXPECT_GT(entry.audio_file.length(), 0);
        EXPECT_GT(entry.transcription.length(), 0);
        EXPECT_GT(entry.normalized_transcription.length(), 0);
    }
}

TEST_F(LJSpeechToolsTest, DatasetPreparatorSaveLoadTest) {
    // Create test metadata
    std::vector<DatasetPreparator::MetadataEntry> metadata;
    
    DatasetPreparator::MetadataEntry entry1;
    entry1.audio_file = "wavs/test1.wav";
    entry1.transcription = "This is the first test.";
    entry1.normalized_transcription = "this is the first test";
    metadata.push_back(entry1);
    
    DatasetPreparator::MetadataEntry entry2;
    entry2.audio_file = "wavs/test2.wav";
    entry2.transcription = "This is the second test!";
    entry2.normalized_transcription = "this is the second test";
    metadata.push_back(entry2);
    
    // Save metadata
    std::string metadata_file = output_dir_ / "test_metadata.csv";
    EXPECT_TRUE(DatasetPreparator::saveMetadata(metadata, metadata_file));
    
    // Load metadata
    auto loaded_metadata = DatasetPreparator::loadMetadata(metadata_file);
    
    EXPECT_EQ(loaded_metadata.size(), 2);
    EXPECT_EQ(loaded_metadata[0].audio_file, "wavs/test1.wav");
    EXPECT_EQ(loaded_metadata[0].transcription, "This is the first test.");
    EXPECT_EQ(loaded_metadata[1].audio_file, "wavs/test2.wav");
    EXPECT_EQ(loaded_metadata[1].transcription, "This is the second test!");
}

TEST_F(LJSpeechToolsTest, DatasetPreparatorValidateTest) {
    // Create a valid dataset structure
    fs::create_directories(output_dir_ / "wavs");
    
    // Create metadata file
    std::ofstream metadata_file(output_dir_ / "metadata.csv");
    metadata_file << "wavs/test1.wav|This is test 1|this is test 1\n";
    metadata_file << "wavs/test2.wav|This is test 2|this is test 2\n";
    metadata_file.close();
    
    EXPECT_TRUE(DatasetPreparator::validateDataset(output_dir_.string()));
}

TEST_F(LJSpeechToolsTest, DatasetPreparatorNormalizeTest) {
    std::string text = "Hello, WORLD! This is a TEST... 123.";
    std::string normalized = DatasetPreparator::normalizeTranscription(text);
    
    EXPECT_EQ(normalized, "hello world this is a test 123");
}

TEST_F(LJSpeechToolsTest, LJSpeechToolsIntegrationTest) {
    LJSpeechTools tools;
    
    EXPECT_TRUE(tools.initialize());
    
    // Test pipeline
    EXPECT_TRUE(tools.runPipeline(
        input_dir_.string(),
        output_dir_.string(),
        true
    ));
    
    // Check that metadata file was created
    std::string metadata_file = output_dir_ / "metadata.csv";
    EXPECT_TRUE(fs::exists(metadata_file));
    
    // Validate the dataset
    EXPECT_TRUE(DatasetPreparator::validateDataset(output_dir_.string()));
}

TEST_F(LJSpeechToolsTest, LJSpeechToolsGettersTest) {
    LJSpeechTools tools;
    tools.initialize();
    
    // Test that all getters return valid instances
    auto& audio_processor = tools.getAudioProcessor();
    auto& transcriber = tools.getTranscriber();
    auto& synthesizer = tools.getSynthesizer();
    auto& dataset_preparator = tools.getDatasetPreparator();
    
    // Suppress unused variable warnings
    (void)audio_processor;
    (void)dataset_preparator;
    
    // Test basic operations
    AudioData audio;
    audio.samples.resize(1000, 0.5f);
    audio.sample_rate = 44100;
    audio.channels = 1;
    audio.duration_seconds = 1.0;
    
    auto result = transcriber.transcribe(audio);
    EXPECT_TRUE(result.success);
    
    auto synth_audio = synthesizer.synthesize("Test text");
    EXPECT_GT(synth_audio.samples.size(), 0);
}