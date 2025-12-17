#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".lib/base-client.hpp"
#include ".types/audio.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class AudioService {
public:
    std::future<ArrayBuffer> requestBinary(const std::string& method, const std::string& path, std::optional<std::any> options);
    std::variant<Blob, std::string> processAudioInput(const std::variant<Blob, Buffer, std::string>& audio);
    bool isBase64String(const std::string& str);
    obj is Buffer isBuffer(const std::any& obj);
    std::future<SpeechResponse> speechConversation(UUID agentId, SpeechConversationParams params);
    Promise< generateSpeech(UUID agentId, SpeechGenerateParams params);
    Promise< synthesizeAudioMessage(UUID agentId, AudioSynthesizeParams params);
    std::future<TranscriptionResponse> transcribe(UUID agentId, TranscribeParams params);
    std::future<SpeechResponse> processSpeech(UUID agentId, const std::variant<Blob, Buffer, std::string>& audio, std::optional<std::unordered_map<std::string, std::any>> metadata);
};


} // namespace elizaos
