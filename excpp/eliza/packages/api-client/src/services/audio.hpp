#pragma once
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
    std::future<ArrayBuffer> requestBinary(const std::string& method, const std::string& path, std::optional<{
      body: any;
      params: Record<string> options, std::optional<Record<string> any>;
      headers, auto string>;
    });
    std::variant<Blob, std::string> processAudioInput(const std::variant<Blob, Buffer, std::string>& audio);
    bool isBase64String(const std::string& str);
    std::future<SpeechResponse> speechConversation(UUID agentId, SpeechConversationParams params);
    void for(auto let i = 0; i < bytes.byteLength; i++);
    std::future<TranscriptionResponse> transcribe(UUID agentId, TranscribeParams params);
    std::future<SpeechResponse> processSpeech(UUID agentId, const std::variant<Blob, Buffer, std::string>& audio, std::optional<Record<string> metadata, auto any>);


} // namespace elizaos
