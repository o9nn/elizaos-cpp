#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"
#include "lib/base-client.hpp"
#include "types/audio.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class AudioService {
public:
    std::future<std::vector<uint8_t>> requestBinary(const std::string& method, const std::string& path, std::optional<std::any> options);
    std::variant<std::vector<uint8_t>, std::string> processAudioInput(const std::variant<std::vector<uint8_t>, std::string>& audio);
    bool isBase64String(const std::string& str);
    obj is Buffer isBuffer(const std::any& obj);
    std::future<SpeechResponse> speechConversation(UUID agentId, SpeechConversationParams params);
    Promise< generateSpeech(UUID agentId, SpeechGenerateParams params);
    Promise< synthesizeAudioMessage(UUID agentId, AudioSynthesizeParams params);
    std::future<TranscriptionResponse> transcribe(UUID agentId, TranscribeParams params);
    std::future<SpeechResponse> processSpeech(UUID agentId, const std::variant<std::vector<uint8_t>, std::string>& audio, std::optional<std::unordered_map<std::string, std::any>> metadata);
};


} // namespace elizaos
