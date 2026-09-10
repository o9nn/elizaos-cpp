#include "elizaos/ljspeechtools.hpp"
#include "elizaos/agentlogger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <cmath>
#include <filesystem>
#include <cstring>
#include <array>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <cctype>
#include <system_error>

#ifdef HAVE_SNDFILE
#include <sndfile.h>
#endif

namespace elizaos {

namespace fs = std::filesystem;

// Global logger instance for the module
static AgentLogger g_logger;

namespace {

std::uint16_t readU16LE(std::istream& input) {
    std::array<unsigned char, 2> bytes{};
    if (!input.read(reinterpret_cast<char*>(bytes.data()), bytes.size())) {
        throw std::runtime_error("truncated 16-bit WAV field");
    }
    return static_cast<std::uint16_t>(bytes[0]) |
           (static_cast<std::uint16_t>(bytes[1]) << 8U);
}

std::uint32_t readU32LE(std::istream& input) {
    std::array<unsigned char, 4> bytes{};
    if (!input.read(reinterpret_cast<char*>(bytes.data()), bytes.size())) {
        throw std::runtime_error("truncated 32-bit WAV field");
    }
    return static_cast<std::uint32_t>(bytes[0]) |
           (static_cast<std::uint32_t>(bytes[1]) << 8U) |
           (static_cast<std::uint32_t>(bytes[2]) << 16U) |
           (static_cast<std::uint32_t>(bytes[3]) << 24U);
}

void writeU16LE(std::ostream& output, std::uint16_t value) {
    const std::array<unsigned char, 2> bytes = {
        static_cast<unsigned char>(value & 0xffU),
        static_cast<unsigned char>((value >> 8U) & 0xffU)
    };
    output.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
}

void writeU32LE(std::ostream& output, std::uint32_t value) {
    const std::array<unsigned char, 4> bytes = {
        static_cast<unsigned char>(value & 0xffU),
        static_cast<unsigned char>((value >> 8U) & 0xffU),
        static_cast<unsigned char>((value >> 16U) & 0xffU),
        static_cast<unsigned char>((value >> 24U) & 0xffU)
    };
    output.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
}

AudioData loadPcmWave(const std::string& file_path) {
    std::ifstream input(file_path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("unable to open audio file");
    }

    char riff[4]{};
    char wave[4]{};
    input.read(riff, sizeof(riff));
    (void)readU32LE(input);
    input.read(wave, sizeof(wave));
    if (!input || std::memcmp(riff, "RIFF", 4) != 0 || std::memcmp(wave, "WAVE", 4) != 0) {
        throw std::runtime_error("not a RIFF/WAVE file");
    }

    std::uint16_t format = 0;
    std::uint16_t channels = 0;
    std::uint16_t bits_per_sample = 0;
    std::uint32_t sample_rate = 0;
    std::vector<unsigned char> payload;
    bool found_format = false;
    bool found_data = false;

    while (input && !(found_format && found_data)) {
        char chunk_id[4]{};
        if (!input.read(chunk_id, sizeof(chunk_id))) break;
        const auto chunk_size = readU32LE(input);
        if (std::memcmp(chunk_id, "fmt ", 4) == 0) {
            if (chunk_size < 16) throw std::runtime_error("invalid WAV format chunk");
            format = readU16LE(input);
            channels = readU16LE(input);
            sample_rate = readU32LE(input);
            (void)readU32LE(input);  // byte rate
            (void)readU16LE(input);  // block align
            bits_per_sample = readU16LE(input);
            if (chunk_size > 16) input.seekg(static_cast<std::streamoff>(chunk_size - 16), std::ios::cur);
            found_format = true;
        } else if (std::memcmp(chunk_id, "data", 4) == 0) {
            payload.resize(chunk_size);
            if (chunk_size > 0 && !input.read(reinterpret_cast<char*>(payload.data()), chunk_size)) {
                throw std::runtime_error("truncated WAV data chunk");
            }
            found_data = true;
        } else {
            input.seekg(static_cast<std::streamoff>(chunk_size), std::ios::cur);
        }
        if (chunk_size & 1U) input.seekg(1, std::ios::cur);
    }

    if (!found_format || !found_data || channels == 0 || sample_rate == 0) {
        throw std::runtime_error("WAV file is missing format or data");
    }
    if (!((format == 1 && (bits_per_sample == 8 || bits_per_sample == 16 ||
                           bits_per_sample == 24 || bits_per_sample == 32)) ||
          (format == 3 && bits_per_sample == 32))) {
        throw std::runtime_error("unsupported WAV encoding");
    }

    const std::size_t bytes_per_sample = bits_per_sample / 8U;
    const std::size_t bytes_per_frame = bytes_per_sample * channels;
    if (bytes_per_sample == 0 || bytes_per_frame / bytes_per_sample != channels ||
        payload.size() % bytes_per_frame != 0) {
        throw std::runtime_error("misaligned WAV data chunk");
    }

    AudioData result;
    result.sample_rate = static_cast<int>(sample_rate);
    result.channels = static_cast<int>(channels);
    result.samples.reserve(payload.size() / bytes_per_sample);
    for (std::size_t offset = 0; offset < payload.size(); offset += bytes_per_sample) {
        float sample = 0.0f;
        if (format == 3) {
            std::uint32_t raw = static_cast<std::uint32_t>(payload[offset]) |
                (static_cast<std::uint32_t>(payload[offset + 1]) << 8U) |
                (static_cast<std::uint32_t>(payload[offset + 2]) << 16U) |
                (static_cast<std::uint32_t>(payload[offset + 3]) << 24U);
            std::memcpy(&sample, &raw, sizeof(sample));
        } else if (bits_per_sample == 8) {
            sample = (static_cast<int>(payload[offset]) - 128) / 128.0f;
        } else {
            std::uint32_t raw = static_cast<std::uint32_t>(payload[offset]) |
                (static_cast<std::uint32_t>(payload[offset + 1]) << 8U);
            if (bits_per_sample >= 24) raw |= static_cast<std::uint32_t>(payload[offset + 2]) << 16U;
            if (bits_per_sample == 32) raw |= static_cast<std::uint32_t>(payload[offset + 3]) << 24U;
            if (bits_per_sample < 32 && (raw & (1U << (bits_per_sample - 1U)))) {
                raw |= ~((1U << bits_per_sample) - 1U);
            }
            std::int32_t value = 0;
            std::memcpy(&value, &raw, sizeof(value));
            const double denominator = std::ldexp(1.0, bits_per_sample - 1);
            sample = static_cast<float>(static_cast<double>(value) / denominator);
        }
        if (!std::isfinite(sample)) throw std::runtime_error("non-finite WAV sample");
        result.samples.push_back(std::clamp(sample, -1.0f, 1.0f));
    }
    if (result.samples.size() % channels != 0) {
        throw std::runtime_error("WAV channel data is incomplete");
    }
    result.duration_seconds = static_cast<double>(result.samples.size() / channels) / sample_rate;
    return result;
}

bool savePcm16Wave(const AudioData& audio, const std::string& file_path) {
    if (audio.sample_rate <= 0 || audio.channels <= 0 || audio.samples.empty() ||
        audio.samples.size() % static_cast<std::size_t>(audio.channels) != 0) {
        return false;
    }
    if (audio.channels > std::numeric_limits<std::uint16_t>::max()) return false;

    const std::uint64_t data_size_64 = audio.samples.size() * sizeof(std::int16_t);
    if (data_size_64 > std::numeric_limits<std::uint32_t>::max() - 36U) return false;
    const auto data_size = static_cast<std::uint32_t>(data_size_64);
    const auto sample_rate = static_cast<std::uint32_t>(audio.sample_rate);
    const auto channels = static_cast<std::uint16_t>(audio.channels);
    const std::uint64_t byte_rate_64 = static_cast<std::uint64_t>(sample_rate) *
                                       channels * sizeof(std::int16_t);
    const std::uint64_t block_align_64 = static_cast<std::uint64_t>(channels) *
                                         sizeof(std::int16_t);
    if (byte_rate_64 > std::numeric_limits<std::uint32_t>::max() ||
        block_align_64 > std::numeric_limits<std::uint16_t>::max()) {
        return false;
    }

    std::ofstream output(file_path, std::ios::binary | std::ios::trunc);
    if (!output) return false;
    output.write("RIFF", 4);
    writeU32LE(output, 36U + data_size);
    output.write("WAVE", 4);
    output.write("fmt ", 4);
    writeU32LE(output, 16U);
    writeU16LE(output, 1U);
    writeU16LE(output, channels);
    writeU32LE(output, sample_rate);
    writeU32LE(output, static_cast<std::uint32_t>(byte_rate_64));
    writeU16LE(output, static_cast<std::uint16_t>(block_align_64));
    writeU16LE(output, 16U);
    output.write("data", 4);
    writeU32LE(output, data_size);
    for (float sample : audio.samples) {
        if (!std::isfinite(sample)) return false;
        const float clamped = std::clamp(sample, -1.0f, 1.0f);
        const auto pcm = static_cast<std::int16_t>(std::lrint(clamped * 32767.0f));
        writeU16LE(output, static_cast<std::uint16_t>(pcm));
    }
    output.flush();
    return output.good();
}

}  // namespace

// AudioProcessor implementation
AudioData AudioProcessor::loadAudioFile(const std::string& file_path) {
    AudioData result;
    
#ifdef HAVE_SNDFILE
    // Real implementation using libsndfile
    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));
    
    SNDFILE* sndfile = sf_open(file_path.c_str(), SFM_READ, &sfinfo);
    if (!sndfile) {
        g_logger.log("Failed to open audio file: " + file_path + " - " + sf_strerror(nullptr), "", "ljspeechtools", LogLevel::ERROR);
        throw std::runtime_error("failed to decode audio file: " + file_path);
    }
    
    // Read file info
    result.sample_rate = sfinfo.samplerate;
    result.channels = sfinfo.channels;
    result.duration_seconds = static_cast<double>(sfinfo.frames) / sfinfo.samplerate;
    
    // Read audio data
    result.samples.resize(sfinfo.frames * sfinfo.channels);
    sf_count_t read_count = sf_readf_float(sndfile, result.samples.data(), sfinfo.frames);
    
    if (read_count != sfinfo.frames) {
        g_logger.log("Warning: Expected to read " + std::to_string(sfinfo.frames) + 
                    " frames, but read " + std::to_string(read_count), "", "ljspeechtools", LogLevel::WARNING);
    }
    
    sf_close(sndfile);
    g_logger.log("Loaded audio file: " + file_path + 
                " (" + std::to_string(result.duration_seconds) + "s, " +
                std::to_string(result.sample_rate) + "Hz, " + 
                std::to_string(result.channels) + " ch)", "", "ljspeechtools", LogLevel::INFO);
#else
    // Dependency-free fallback supports standard RIFF/WAVE PCM and float data.
    result = loadPcmWave(file_path);
    g_logger.log("Loaded WAV audio file: " + file_path +
                 " (" + std::to_string(result.duration_seconds) + "s, " +
                 std::to_string(result.sample_rate) + "Hz, " +
                 std::to_string(result.channels) + " ch)",
                 "", "ljspeechtools", LogLevel::INFO);
#endif
    
    return result;
}

bool AudioProcessor::saveAudioFile(const AudioData& audio, const std::string& file_path) {
    if (file_path.empty() || audio.sample_rate <= 0 || audio.channels <= 0 ||
        audio.samples.empty() ||
        audio.samples.size() % static_cast<std::size_t>(audio.channels) != 0) {
        return false;
    }
#ifdef HAVE_SNDFILE
    // Real implementation using libsndfile
    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));
    
    sfinfo.samplerate = audio.sample_rate;
    sfinfo.channels = audio.channels;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16; // 16-bit PCM WAV
    
    SNDFILE* sndfile = sf_open(file_path.c_str(), SFM_WRITE, &sfinfo);
    if (!sndfile) {
        g_logger.log("Failed to create audio file: " + file_path + " - " + sf_strerror(nullptr), "", "ljspeechtools", LogLevel::ERROR);
        return false;
    }
    
    sf_count_t frames_to_write = audio.samples.size() / audio.channels;
    sf_count_t written = sf_writef_float(sndfile, audio.samples.data(), frames_to_write);
    
    sf_close(sndfile);
    
    if (written != frames_to_write) {
        g_logger.log("Warning: Expected to write " + std::to_string(frames_to_write) + 
                    " frames, but wrote " + std::to_string(written), "", "ljspeechtools", LogLevel::WARNING);
        return false;
    }
    
    g_logger.log("Saved audio file: " + file_path + 
                " (" + std::to_string(audio.duration_seconds) + "s, " +
                std::to_string(audio.sample_rate) + "Hz, " + 
                std::to_string(audio.channels) + " ch)", "", "ljspeechtools", LogLevel::INFO);
    return true;
#else
    const bool saved = savePcm16Wave(audio, file_path);
    g_logger.log(std::string(saved ? "Saved" : "Failed to save") +
                 " WAV audio file: " + file_path,
                 "", "ljspeechtools", saved ? LogLevel::INFO : LogLevel::ERROR);
    return saved;
#endif
}

std::vector<AudioData> AudioProcessor::splitOnSilence(
    const AudioData& audio,
    int min_silence_len_ms,
    float silence_thresh_db,
    int keep_silence_ms
) {
    std::vector<AudioData> chunks;
    if (audio.sample_rate <= 0 || audio.channels <= 0 || audio.samples.empty() ||
        audio.samples.size() % static_cast<std::size_t>(audio.channels) != 0 ||
        min_silence_len_ms < 0 || keep_silence_ms < 0) {
        return chunks;
    }

    // Convert parameters
    const std::size_t channel_count = static_cast<std::size_t>(audio.channels);
    const std::size_t frame_count = audio.samples.size() / channel_count;
    const std::size_t min_silence_frames = static_cast<std::size_t>(
        (static_cast<long long>(min_silence_len_ms) * audio.sample_rate) / 1000);
    const std::size_t keep_silence_frames = static_cast<std::size_t>(
        (static_cast<long long>(keep_silence_ms) * audio.sample_rate) / 1000);
    const float silence_thresh_linear = std::pow(10.0f, silence_thresh_db / 20.0f);

    // A frame is silent only if every channel is below the threshold.
    std::vector<bool> is_silent(frame_count, true);
    for (std::size_t frame = 0; frame < frame_count; ++frame) {
        for (std::size_t channel = 0; channel < channel_count; ++channel) {
            if (std::abs(audio.samples[frame * channel_count + channel]) >= silence_thresh_linear) {
                is_silent[frame] = false;
                break;
            }
        }
    }
    
    // Find continuous silence regions
    std::vector<std::pair<size_t, size_t>> silence_regions;
    size_t start = 0;
    bool in_silence = false;
    
    for (size_t i = 0; i < is_silent.size(); ++i) {
        if (!in_silence && is_silent[i]) {
            start = i;
            in_silence = true;
        } else if (in_silence && !is_silent[i]) {
            if (i - start >= min_silence_frames) {
                silence_regions.emplace_back(start, i);
            }
            in_silence = false;
        }
    }
    
    // Handle case where audio ends in silence
    if (in_silence && is_silent.size() - start >= min_silence_frames) {
        silence_regions.emplace_back(start, is_silent.size());
    }
    
    // Split at silence regions
    size_t last_end = 0;
    for (const auto& silence_region : silence_regions) {
        size_t chunk_start = last_end;
        const size_t chunk_end = std::min(frame_count, silence_region.first + keep_silence_frames);

        if (chunk_end > chunk_start) {
            AudioData chunk;
            chunk.sample_rate = audio.sample_rate;
            chunk.channels = audio.channels;
            chunk.samples.assign(
                audio.samples.begin() + static_cast<std::ptrdiff_t>(chunk_start * channel_count),
                audio.samples.begin() + static_cast<std::ptrdiff_t>(chunk_end * channel_count));
            chunk.duration_seconds = static_cast<double>(chunk_end - chunk_start) / audio.sample_rate;
            chunks.push_back(chunk);
        }

        last_end = silence_region.second > keep_silence_frames
            ? silence_region.second - keep_silence_frames
            : 0;
    }
    
    // Add final chunk if there's audio after the last silence
    if (last_end < frame_count) {
        AudioData chunk;
        chunk.sample_rate = audio.sample_rate;
        chunk.channels = audio.channels;
        chunk.samples.assign(
            audio.samples.begin() + static_cast<std::ptrdiff_t>(last_end * channel_count),
            audio.samples.end());
        chunk.duration_seconds = static_cast<double>(frame_count - last_end) / audio.sample_rate;
        chunks.push_back(chunk);
    }
    
    // If no splits were made, return the original audio
    if (chunks.empty()) {
        chunks.push_back(audio);
    }
    
    g_logger.log("Split audio into " + std::to_string(chunks.size()) + " chunks using silence detection", "", "ljspeechtools", LogLevel::INFO);
    return chunks;
}

std::vector<AudioData> AudioProcessor::filterByDuration(
    const std::vector<AudioData>& chunks,
    double min_duration,
    double max_duration
) {
    std::vector<AudioData> filtered;
    
    for (const auto& chunk : chunks) {
        if (chunk.duration_seconds >= min_duration && 
            chunk.duration_seconds <= max_duration) {
            filtered.push_back(chunk);
        }
    }
    
    return filtered;
}

AudioData AudioProcessor::normalize(const AudioData& audio) {
    AudioData result = audio;
    
    // Find maximum amplitude
    float max_amplitude = 0.0f;
    for (float sample : result.samples) {
        max_amplitude = std::max(max_amplitude, std::abs(sample));
    }
    
    // Normalize to 0.9 to avoid clipping
    if (max_amplitude > 0.0f) {
        float scale = 0.9f / max_amplitude;
        for (float& sample : result.samples) {
            sample *= scale;
        }
    }
    
    return result;
}

AudioData AudioProcessor::convertFormat(
    const AudioData& audio,
    int target_sample_rate,
    int target_channels
) {
    AudioData result;
    if (audio.sample_rate <= 0 || audio.channels <= 0 || target_sample_rate <= 0 ||
        target_channels <= 0 || audio.samples.empty() ||
        audio.samples.size() % static_cast<std::size_t>(audio.channels) != 0) {
        return result;
    }

    const std::size_t input_channels = static_cast<std::size_t>(audio.channels);
    const std::size_t input_frames = audio.samples.size() / input_channels;
    const std::size_t output_channels = static_cast<std::size_t>(target_channels);

    std::vector<float> channel_converted(input_frames * output_channels);
    for (std::size_t frame = 0; frame < input_frames; ++frame) {
        if (target_channels == 1) {
            double sum = 0.0;
            for (std::size_t channel = 0; channel < input_channels; ++channel) {
                sum += audio.samples[frame * input_channels + channel];
            }
            channel_converted[frame] = static_cast<float>(sum / input_channels);
        } else {
            for (std::size_t channel = 0; channel < output_channels; ++channel) {
                const std::size_t source_channel = audio.channels == 1
                    ? 0
                    : std::min(channel, input_channels - 1);
                channel_converted[frame * output_channels + channel] =
                    audio.samples[frame * input_channels + source_channel];
            }
        }
    }

    const double ratio = static_cast<double>(target_sample_rate) / audio.sample_rate;
    const std::size_t output_frames = std::max<std::size_t>(
        1, static_cast<std::size_t>(std::llround(input_frames * ratio)));
    result.samples.resize(output_frames * output_channels);
    for (std::size_t frame = 0; frame < output_frames; ++frame) {
        const double source_position = std::min(
            static_cast<double>(input_frames - 1), static_cast<double>(frame) / ratio);
        const auto frame0 = static_cast<std::size_t>(source_position);
        const auto frame1 = std::min(frame0 + 1, input_frames - 1);
        const float fraction = static_cast<float>(source_position - frame0);
        for (std::size_t channel = 0; channel < output_channels; ++channel) {
            const float first = channel_converted[frame0 * output_channels + channel];
            const float second = channel_converted[frame1 * output_channels + channel];
            result.samples[frame * output_channels + channel] = first + (second - first) * fraction;
        }
    }

    result.channels = target_channels;
    result.sample_rate = target_sample_rate;
    result.duration_seconds = static_cast<double>(output_frames) / target_sample_rate;
    
    g_logger.log("Converted audio format: " + 
                std::to_string(audio.sample_rate) + "Hz/" + std::to_string(audio.channels) + "ch -> " +
                std::to_string(target_sample_rate) + "Hz/" + std::to_string(target_channels) + "ch", 
                "", "ljspeechtools", LogLevel::INFO);
    return result;
}

// SpeechTranscriber implementation
namespace {

constexpr int kMaximumSampleRate = 768000;
constexpr int kMaximumChannels = 256;
constexpr std::size_t kMaximumTranscriptionSamples = 500000000U;

TranscriptionResult transcriptionFailure(std::string message) {
    TranscriptionResult result;
    result.error_message = std::move(message);
    return result;
}

bool isBlank(const std::string& text) {
    return std::all_of(text.begin(), text.end(), [](unsigned char character) {
        return std::isspace(character) != 0;
    });
}

std::string validateTranscriptionInput(const AudioData& audio) {
    if (audio.sample_rate <= 0 || audio.sample_rate > kMaximumSampleRate) {
        return "Invalid audio sample rate";
    }
    if (audio.channels <= 0 || audio.channels > kMaximumChannels) {
        return "Invalid audio channel count";
    }
    if (audio.samples.empty()) {
        return "Empty audio data";
    }
    if (audio.samples.size() > kMaximumTranscriptionSamples) {
        return "Audio sample count exceeds transcription limit";
    }
    const auto channel_count = static_cast<std::size_t>(audio.channels);
    if (audio.samples.size() % channel_count != 0U) {
        return "Audio samples do not contain complete interleaved frames";
    }
    if (!std::all_of(audio.samples.begin(), audio.samples.end(), [](float sample) {
            return std::isfinite(sample) && sample >= -1.0f && sample <= 1.0f;
        })) {
        return "Audio samples must be finite and within [-1, 1]";
    }
    if (!std::isfinite(audio.duration_seconds) || audio.duration_seconds < 0.0) {
        return "Invalid audio duration";
    }
    return {};
}

std::string joinSegmentText(const std::vector<TranscriptionSegment>& segments) {
    std::string text;
    for (const auto& segment : segments) {
        if (segment.text.empty()) {
            continue;
        }
        if (!text.empty()) {
            text.push_back(' ');
        }
        text += segment.text;
    }
    return text;
}

TranscriptionResult validateBackendResult(TranscriptionResult result, std::size_t frame_count) {
    if (!result.success) {
        if (result.error_message.empty()) {
            result.error_message = "Transcription backend failed without an error message";
        }
        result.text.clear();
        result.confidence = 0.0;
        result.segments.clear();
        return result;
    }
    if (!result.error_message.empty()) {
        return transcriptionFailure("Malformed transcription backend result: success included an error");
    }
    if (!std::isfinite(result.confidence) || result.confidence < 0.0 || result.confidence > 1.0) {
        return transcriptionFailure("Malformed transcription backend result: invalid confidence");
    }

    std::size_t previous_end = 0;
    std::vector<TranscriptionSegment> normalized_segments;
    normalized_segments.reserve(result.segments.size());
    for (const auto& segment : result.segments) {
        if (segment.begin_frame >= segment.end_frame || segment.end_frame > frame_count ||
            segment.begin_frame < previous_end || segment.text.empty() ||
            !std::isfinite(segment.confidence) || segment.confidence < 0.0 ||
            segment.confidence > 1.0) {
            return transcriptionFailure("Malformed transcription backend result: invalid or unordered segment");
        }

        // Backends sometimes repeat an overlap segment verbatim. Preserve time
        // ordering while dropping only exact adjacent text duplicates.
        if (!normalized_segments.empty() &&
            normalized_segments.back().text == segment.text) {
            normalized_segments.back().end_frame = segment.end_frame;
            normalized_segments.back().confidence =
                std::min(normalized_segments.back().confidence, segment.confidence);
        } else {
            normalized_segments.push_back(segment);
        }
        previous_end = segment.end_frame;
    }
    result.segments = std::move(normalized_segments);

    if (!result.segments.empty()) {
        const std::string canonical_text = joinSegmentText(result.segments);
        if (!result.text.empty() && result.text != canonical_text) {
            return transcriptionFailure(
                "Malformed transcription backend result: text does not match ordered segments");
        }
        result.text = canonical_text;
    }
    if (result.text.empty() || isBlank(result.text)) {
        return transcriptionFailure("Malformed transcription backend result: empty transcript");
    }
    return result;
}

}  // namespace

class SpeechTranscriber::Impl {
public:
    explicit Impl(std::shared_ptr<TranscriptionBackend> backend)
        : backend_(std::move(backend)) {}

    TranscriptionResult transcribe(const AudioData& audio) const {
        const std::string validation_error = validateTranscriptionInput(audio);
        if (!validation_error.empty()) {
            return transcriptionFailure(validation_error);
        }
        if (model_name_.empty() || isBlank(model_name_)) {
            return transcriptionFailure("Transcription model name must not be empty");
        }
        if (!backend_) {
            return transcriptionFailure(
                "No transcription backend configured; inject a TranscriptionBackend");
        }

        TranscriptionRequest request;
        request.audio = audio;
        const auto channel_count = static_cast<std::size_t>(request.audio.channels);
        const auto frame_count = request.audio.samples.size() / channel_count;
        request.audio.duration_seconds = static_cast<double>(frame_count) /
                                         request.audio.sample_rate;
        request.model_name = model_name_;

        try {
            return validateBackendResult(backend_->transcribe(request), frame_count);
        } catch (const std::exception& error) {
            return transcriptionFailure(
                "Transcription backend exception: " + std::string(error.what()));
        } catch (...) {
            return transcriptionFailure("Transcription backend raised an unknown exception");
        }
    }

    TranscriptionResult transcribeFile(const std::string& file_path) const {
        if (file_path.empty()) {
            return transcriptionFailure("Audio file path must not be empty");
        }
        std::error_code error;
        if (!fs::is_regular_file(file_path, error)) {
            return transcriptionFailure("Audio file not found or not regular: " + file_path);
        }
        try {
            return transcribe(AudioProcessor::loadAudioFile(file_path));
        } catch (const std::exception& exception) {
            return transcriptionFailure(
                "Error loading audio file: " + std::string(exception.what()));
        }
    }

    std::string model_name_ = "default";
    std::shared_ptr<TranscriptionBackend> backend_;
};

SpeechTranscriber::SpeechTranscriber(std::shared_ptr<TranscriptionBackend> backend)
    : impl_(std::make_unique<Impl>(std::move(backend))) {}
SpeechTranscriber::~SpeechTranscriber() = default;

TranscriptionResult SpeechTranscriber::transcribe(const AudioData& audio) {
    return impl_->transcribe(audio);
}

TranscriptionResult SpeechTranscriber::transcribeFile(const std::string& file_path) {
    return impl_->transcribeFile(file_path);
}

std::vector<std::pair<std::string, TranscriptionResult>> SpeechTranscriber::transcribeBatch(
    const std::vector<std::string>& file_paths
) {
    std::vector<std::pair<std::string, TranscriptionResult>> results;
    results.reserve(file_paths.size());
    for (const auto& path : file_paths) {
        results.emplace_back(path, transcribeFile(path));
    }
    return results;
}

void SpeechTranscriber::setTranscriptionModel(const std::string& model_name) {
    impl_->model_name_ = model_name;
}

void SpeechTranscriber::setBackend(std::shared_ptr<TranscriptionBackend> backend) {
    impl_->backend_ = std::move(backend);
}

bool SpeechTranscriber::hasBackend() const {
    return static_cast<bool>(impl_->backend_);
}

// SpeechSynthesizer implementation
class SpeechSynthesizer::Impl {
public:
    std::string model_name_ = "default";
    
    AudioData synthesize(const std::string& text, const SynthesisConfig& config) {
        AudioData result;
        
        if (text.empty()) {
            g_logger.log("Warning: Empty text provided for synthesis", "", "ljspeechtools", LogLevel::WARNING);
            return result;
        }
        if (config.sample_rate <= 0 || config.sample_rate > kMaximumSampleRate ||
            config.channels <= 0 || config.channels > kMaximumChannels ||
            !std::isfinite(config.speed) || config.speed <= 0.0f ||
            !std::isfinite(config.pitch) || config.pitch <= 0.0f) {
            g_logger.log("Invalid synthesis configuration", "", "ljspeechtools", LogLevel::ERROR);
            return result;
        }

        // Local deterministic waveform generation, not a speech/transcription backend.
        double base_duration = text.length() * 0.08; // ~80ms per character
        base_duration *= (1.0 / config.speed); // Adjust for speed
        
        int duration_samples = static_cast<int>(base_duration * config.sample_rate);
        result.samples.resize(duration_samples * config.channels);
        result.sample_rate = config.sample_rate;
        result.channels = config.channels;
        result.duration_seconds = base_duration;
        
        // Generate more sophisticated waveform based on text content
        std::hash<std::string> hash_fn;
        size_t text_hash = hash_fn(text);
        
        // Use text hash to determine voice characteristics
        constexpr double kPi = 3.14159265358979323846;
        const double base_freq =
            (200.0 + static_cast<double>(text_hash % 200U)) *
            static_cast<double>(config.pitch);
        
        // Generate speech-like waveform with multiple harmonics
        for (int i = 0; i < duration_samples; ++i) {
            const double t = static_cast<double>(i) /
                             static_cast<double>(config.sample_rate);
            
            // Add formant-like frequencies
            double sample = 0.0;
            sample += 0.4 * std::sin(2.0 * kPi * base_freq * t); // Fundamental
            sample += 0.2 * std::sin(2.0 * kPi * base_freq * 2.0 * t); // Second harmonic
            sample += 0.1 * std::sin(2.0 * kPi * base_freq * 3.0 * t); // Third harmonic
            
            // Add some variation based on text content
            const double text_variation = std::sin(
                2.0 * kPi * t *
                static_cast<double>(text_hash % 50U + 10U));
            sample += 0.1 * text_variation;
            
            // Add amplitude envelope (attack-sustain-release)
            double envelope = 1.0;
            constexpr double attack_time = 0.1;
            constexpr double release_time = 0.2;
            
            if (t < attack_time) {
                envelope = t / attack_time;
            } else if (t > base_duration - release_time) {
                envelope = (base_duration - t) / release_time;
            }
            
            sample *= envelope * 0.3; // Overall amplitude scaling
            
            // Fill channels
            for (int ch = 0; ch < config.channels; ++ch) {
                result.samples[i * config.channels + ch] =
                    static_cast<float>(sample);
            }
        }
        
        g_logger.log("Synthesized text (" + std::to_string(text.length()) + " chars, " +
                    std::to_string(base_duration) + "s): " + text.substr(0, 50) + 
                    (text.length() > 50 ? "..." : ""), "", "ljspeechtools", LogLevel::INFO);
        return result;
    }
};

SpeechSynthesizer::SpeechSynthesizer() : impl_(std::make_unique<Impl>()) {}
SpeechSynthesizer::~SpeechSynthesizer() = default;

AudioData SpeechSynthesizer::synthesize(const std::string& text, const SynthesisConfig& config) {
    return impl_->synthesize(text, config);
}

bool SpeechSynthesizer::synthesizeToFile(
    const std::string& text,
    const std::string& output_path,
    const SynthesisConfig& config
) {
    auto audio = synthesize(text, config);
    return AudioProcessor::saveAudioFile(audio, output_path);
}

void SpeechSynthesizer::setSynthesisModel(const std::string& model_name) {
    impl_->model_name_ = model_name;
}

std::vector<std::string> SpeechSynthesizer::getAvailableVoices() {
    return {"default", "female", "male", "child"};
}

// DatasetPreparator implementation
std::vector<DatasetPreparator::MetadataEntry> DatasetPreparator::createDataset(
    const std::string& input_dir,
    const std::string& output_dir,
    bool split_long_audio,
    bool filter_short_audio
) {
    SpeechTranscriber transcriber;
    return createDataset(input_dir, output_dir, transcriber, split_long_audio, filter_short_audio);
}

std::vector<DatasetPreparator::MetadataEntry> DatasetPreparator::createDataset(
    const std::string& input_dir,
    const std::string& output_dir,
    SpeechTranscriber& transcriber,
    bool split_long_audio,
    bool filter_short_audio
) {
    std::vector<MetadataEntry> metadata;
    std::error_code error;
    if (!fs::is_directory(input_dir, error)) {
        g_logger.log("Dataset input directory is not readable: " + input_dir,
                     "", "ljspeechtools", LogLevel::ERROR);
        return metadata;
    }
    if (output_dir.empty()) {
        g_logger.log("Dataset output directory must not be empty",
                     "", "ljspeechtools", LogLevel::ERROR);
        return metadata;
    }

    const fs::path wavs_directory = fs::path(output_dir) / "wavs";
    if (!fs::create_directories(wavs_directory, error) && error) {
        g_logger.log("Unable to create dataset output directory: " + error.message(),
                     "", "ljspeechtools", LogLevel::ERROR);
        return metadata;
    }

    std::vector<fs::path> input_files;
    for (fs::recursive_directory_iterator iterator(
             input_dir, fs::directory_options::skip_permission_denied, error), end;
         iterator != end;
         iterator.increment(error)) {
        if (error) {
            g_logger.log("Error enumerating dataset input: " + error.message(),
                         "", "ljspeechtools", LogLevel::ERROR);
            error.clear();
            continue;
        }
        if (!iterator->is_regular_file(error)) {
            error.clear();
            continue;
        }
        std::string extension = iterator->path().extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(),
            [](unsigned char character) { return static_cast<char>(std::tolower(character)); });
#ifdef HAVE_SNDFILE
        const bool supported = extension == ".wav" || extension == ".flac" ||
                               extension == ".ogg";
#else
        const bool supported = extension == ".wav";
#endif
        if (supported) {
            input_files.push_back(iterator->path());
        }
    }
    std::sort(input_files.begin(), input_files.end());

    std::size_t file_counter = 0;
    for (const auto& input_file : input_files) {
        try {
            const AudioData audio = AudioProcessor::loadAudioFile(input_file.string());
            std::vector<AudioData> chunks =
                split_long_audio && audio.duration_seconds > 12.0
                    ? AudioProcessor::splitOnSilence(audio, 1500, -60.0f, 250)
                    : std::vector<AudioData>{audio};

            for (std::size_t chunk_index = 0; chunk_index < chunks.size(); ++chunk_index) {
                const AudioData& source_chunk = chunks[chunk_index];
                if ((filter_short_audio && source_chunk.duration_seconds < 1.0) ||
                    source_chunk.duration_seconds > 12.0) {
                    continue;
                }

                AudioData chunk = AudioProcessor::convertFormat(
                    AudioProcessor::normalize(source_chunk), 22050, 1);
                if (chunk.samples.empty()) {
                    continue;
                }

                const TranscriptionResult transcription = transcriber.transcribe(chunk);
                if (!transcription.success) {
                    g_logger.log("Skipping untranscribed audio " + input_file.string() +
                                 ": " + transcription.error_message,
                                 "", "ljspeechtools", LogLevel::WARNING);
                    continue;
                }

                const std::string output_filename = "sample_" +
                    std::to_string(file_counter) + "_" + std::to_string(chunk_index) + ".wav";
                const fs::path final_path = wavs_directory / output_filename;
                fs::path staged_path = final_path;
                staged_path += ".tmp";
                fs::remove(staged_path, error);
                error.clear();
                if (!AudioProcessor::saveAudioFile(chunk, staged_path.string())) {
                    fs::remove(staged_path, error);
                    error.clear();
                    continue;
                }
                fs::rename(staged_path, final_path, error);
                if (error) {
                    g_logger.log("Unable to commit dataset audio: " + error.message(),
                                 "", "ljspeechtools", LogLevel::ERROR);
                    fs::remove(staged_path, error);
                    error.clear();
                    continue;
                }

                MetadataEntry entry;
                entry.audio_file = (fs::path("wavs") / output_filename).generic_string();
                entry.transcription = transcription.text;
                entry.normalized_transcription = normalizeTranscription(transcription.text);
                if (entry.normalized_transcription.empty()) {
                    fs::remove(final_path, error);
                    error.clear();
                    continue;
                }
                metadata.push_back(std::move(entry));
            }
        } catch (const std::exception& exception) {
            g_logger.log("Error processing file " + input_file.string() + ": " + exception.what(),
                         "", "ljspeechtools", LogLevel::ERROR);
        }
        ++file_counter;
    }

    g_logger.log("Processed " + std::to_string(file_counter) + " audio files, created " +
                 std::to_string(metadata.size()) + " dataset entries",
                 "", "ljspeechtools", LogLevel::INFO);
    return metadata;
}

bool DatasetPreparator::saveMetadata(
    const std::vector<MetadataEntry>& metadata,
    const std::string& output_path
) {
    if (output_path.empty()) {
        return false;
    }
    for (const auto& entry : metadata) {
        if (entry.audio_file.empty() || entry.transcription.empty() ||
            entry.normalized_transcription.empty() ||
            entry.audio_file.find_first_of("|\r\n") != std::string::npos ||
            entry.transcription.find_first_of("|\r\n") != std::string::npos ||
            entry.normalized_transcription.find_first_of("|\r\n") != std::string::npos) {
            return false;
        }
    }

    const fs::path final_path(output_path);
    fs::path staged_path = final_path;
    staged_path += ".tmp";
    std::error_code error;
    fs::remove(staged_path, error);
    error.clear();
    {
        std::ofstream file(staged_path, std::ios::out | std::ios::trunc);
        if (!file) {
            return false;
        }
        for (const auto& entry : metadata) {
            file << entry.audio_file << '|' << entry.transcription << '|'
                 << entry.normalized_transcription << '\n';
        }
        file.flush();
        if (!file.good()) {
            file.close();
            fs::remove(staged_path, error);
            return false;
        }
    }
    fs::rename(staged_path, final_path, error);
    if (error) {
        fs::remove(staged_path, error);
        return false;
    }
    g_logger.log("Saved metadata to: " + output_path, "", "ljspeechtools", LogLevel::INFO);
    return true;
}

std::vector<DatasetPreparator::MetadataEntry> DatasetPreparator::loadMetadata(
    const std::string& input_path
) {
    std::vector<MetadataEntry> metadata;
    std::ifstream file(input_path);
    
    if (!file.is_open()) {
        g_logger.log("Cannot open metadata file: " + input_path, "", "ljspeechtools", LogLevel::ERROR);
        return metadata;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string part;
        MetadataEntry entry;
        
        if (std::getline(ss, part, '|')) entry.audio_file = part;
        if (std::getline(ss, part, '|')) entry.transcription = part;
        if (std::getline(ss, part, '|')) entry.normalized_transcription = part;
        
        metadata.push_back(entry);
    }
    
    file.close();
    g_logger.log("Loaded metadata from: " + input_path, "", "ljspeechtools", LogLevel::INFO);
    return metadata;
}

bool DatasetPreparator::validateDataset(const std::string& dataset_dir) {
    const fs::path root(dataset_dir);
    const fs::path metadata_path = root / "metadata.csv";
    const fs::path wavs_directory = root / "wavs";
    std::error_code error;
    if (!fs::is_regular_file(metadata_path, error) ||
        !fs::is_directory(wavs_directory, error)) {
        return false;
    }

    const auto metadata = loadMetadata(metadata_path.string());
    if (metadata.empty()) {
        return false;
    }
    for (const auto& entry : metadata) {
        const fs::path relative(entry.audio_file);
        if (relative.empty() || relative.is_absolute() ||
            relative.lexically_normal().string().rfind("..", 0) == 0 ||
            entry.transcription.empty() || entry.normalized_transcription.empty()) {
            return false;
        }
        const fs::path audio_path = root / relative;
        if (!fs::is_regular_file(audio_path, error)) {
            return false;
        }
        try {
            const AudioData audio = AudioProcessor::loadAudioFile(audio_path.string());
            if (audio.samples.empty()) {
                return false;
            }
        } catch (...) {
            return false;
        }
    }
    return true;
}

std::string DatasetPreparator::normalizeTranscription(const std::string& text) {
    std::string normalized = text;
    
    // Convert to lowercase
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    
    // Replace common abbreviations and numbers
    std::vector<std::pair<std::regex, std::string>> replacements = {
        {std::regex("\\bdr\\."), "doctor"},
        {std::regex("\\bmr\\."), "mister"},
        {std::regex("\\bmrs\\."), "missus"},
        {std::regex("\\bms\\."), "miss"},
        {std::regex("\\bst\\."), "saint"},
        {std::regex("\\bave\\."), "avenue"},
        {std::regex("\\brd\\."), "road"},
        {std::regex("\\betc\\."), "etcetera"},
        {std::regex("\\be\\.g\\."), "for example"},
        {std::regex("\\bi\\.e\\."), "that is"},
        {std::regex("\\b1st\\b"), "first"},
        {std::regex("\\b2nd\\b"), "second"},
        {std::regex("\\b3rd\\b"), "third"},
        {std::regex("\\b([0-9]+)th\\b"), "$1th"},
        // Add more number replacements as needed
        {std::regex("\\b0\\b"), "zero"},
        {std::regex("\\b1\\b"), "one"},
        {std::regex("\\b2\\b"), "two"},
        {std::regex("\\b3\\b"), "three"},
        {std::regex("\\b4\\b"), "four"},
        {std::regex("\\b5\\b"), "five"},
        {std::regex("\\b6\\b"), "six"},
        {std::regex("\\b7\\b"), "seven"},
        {std::regex("\\b8\\b"), "eight"},
        {std::regex("\\b9\\b"), "nine"},
    };
    
    for (const auto& replacement : replacements) {
        normalized = std::regex_replace(normalized, replacement.first, replacement.second);
    }
    
    // Remove punctuation except apostrophes (but be more careful with contractions)
    std::regex punct_regex("[^a-zA-Z0-9\\s']");
    normalized = std::regex_replace(normalized, punct_regex, "");
    
    // Handle contractions more carefully
    std::vector<std::pair<std::regex, std::string>> contractions = {
        {std::regex("\\bcan't\\b"), "cannot"},
        {std::regex("\\bwon't\\b"), "will not"},
        {std::regex("\\bshan't\\b"), "shall not"},
        {std::regex("\\bn't\\b"), " not"},  // general n't -> not
        {std::regex("\\b're\\b"), " are"},   // 're -> are
        {std::regex("\\b've\\b"), " have"},  // 've -> have
        {std::regex("\\b'll\\b"), " will"},  // 'll -> will
        {std::regex("\\b'd\\b"), " would"},  // 'd -> would (simplified)
        {std::regex("\\b'm\\b"), " am"},     // 'm -> am
        {std::regex("\\b's\\b"), " is"},     // 's -> is (simplified)
    };
    
    for (const auto& contraction : contractions) {
        normalized = std::regex_replace(normalized, contraction.first, contraction.second);
    }
    
    // Replace multiple spaces with single space
    std::regex space_regex("\\s+");
    normalized = std::regex_replace(normalized, space_regex, " ");
    
    // Trim whitespace
    const auto first = normalized.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos) {
        return {};
    }
    normalized.erase(0, first);
    normalized.erase(normalized.find_last_not_of(" \t\n\r\f\v") + 1);
    
    return normalized;
}

// LJSpeechTools implementation
class LJSpeechTools::Impl {
public:
    AudioProcessor audio_processor_;
    SpeechTranscriber transcriber_;
    SpeechSynthesizer synthesizer_;
    DatasetPreparator dataset_preparator_;
    
    bool initialized_ = false;
    
    bool initialize(const std::string& config_path) {
        g_logger.log("Initializing LJSpeechTools", "", "ljspeechtools", LogLevel::INFO);
        
        if (!config_path.empty()) {
            g_logger.log("Using config file: " + config_path, "", "ljspeechtools", LogLevel::INFO);
        }
        
        initialized_ = true;
        return true;
    }
    
    bool runPipeline(const std::string& input_dir, const std::string& output_dir, bool verbose) {
        if (!initialized_) {
            g_logger.log("LJSpeechTools not initialized", "", "ljspeechtools", LogLevel::ERROR);
            return false;
        }
        
        g_logger.log("Running LJSpeechTools pipeline", "", "ljspeechtools", LogLevel::INFO);
        g_logger.log("Input directory: " + input_dir, "", "ljspeechtools", LogLevel::INFO);
        g_logger.log("Output directory: " + output_dir, "", "ljspeechtools", LogLevel::INFO);
        
        // Create dataset
        auto metadata = DatasetPreparator::createDataset(
            input_dir, output_dir, transcriber_, true, true);
        if (metadata.empty()) {
            g_logger.log("Pipeline produced no truthfully transcribed entries",
                         "", "ljspeechtools", LogLevel::ERROR);
            return false;
        }

        // Save metadata
        std::string metadata_path = output_dir + "/metadata.csv";
        bool saved = dataset_preparator_.saveMetadata(metadata, metadata_path);
        
        if (verbose && saved) {
            g_logger.log("Pipeline completed successfully", "", "ljspeechtools", LogLevel::INFO);
            g_logger.log("Generated " + std::to_string(metadata.size()) + " metadata entries", "", "ljspeechtools", LogLevel::INFO);
        }

        return saved;
    }
};

LJSpeechTools::LJSpeechTools() : impl_(std::make_unique<Impl>()) {}
LJSpeechTools::~LJSpeechTools() = default;

bool LJSpeechTools::initialize(const std::string& config_path) {
    return impl_->initialize(config_path);
}

bool LJSpeechTools::runPipeline(
    const std::string& input_dir,
    const std::string& output_dir,
    bool verbose
) {
    return impl_->runPipeline(input_dir, output_dir, verbose);
}

AudioProcessor& LJSpeechTools::getAudioProcessor() {
    return impl_->audio_processor_;
}

SpeechTranscriber& LJSpeechTools::getTranscriber() {
    return impl_->transcriber_;
}

SpeechSynthesizer& LJSpeechTools::getSynthesizer() {
    return impl_->synthesizer_;
}

DatasetPreparator& LJSpeechTools::getDatasetPreparator() {
    return impl_->dataset_preparator_;
}

} // namespace elizaos
