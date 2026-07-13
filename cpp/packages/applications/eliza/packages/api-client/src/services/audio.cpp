#include "audio.hpp"

#include <utility>

namespace elizaos {
namespace eliza_api_client {

namespace {
std::string stringValueOrDefault(const nlohmann::json& config,
                                 const std::string& camelKey,
                                 const std::string& snakeKey,
                                 const std::string& defaultValue) {
    const auto camelIt = config.find(camelKey);
    if (camelIt != config.end() && camelIt->is_string() && !camelIt->get<std::string>().empty()) {
        return camelIt->get<std::string>();
    }

    const auto snakeIt = config.find(snakeKey);
    if (snakeIt != config.end() && snakeIt->is_string() && !snakeIt->get<std::string>().empty()) {
        return snakeIt->get<std::string>();
    }

    return defaultValue;
}

bool hasNonEmptyString(const nlohmann::json& value, const std::string& key) {
    const auto it = value.find(key);
    return it != value.end() && it->is_string() && !it->get<std::string>().empty();
}

std::string trimTrailingSlashes(std::string path) {
    while (path.size() > 1 && path.back() == '/') {
        path.pop_back();
    }
    return path;
}
} // namespace

Audio::Audio(std::shared_ptr<BaseClient> client)
    : client_(std::move(client)) {}

bool Audio::initialize(const nlohmann::json& config) {
    if (initialized_) {
        return true;
    }

    if (!config.is_object()) {
        setLastError("invalid_config", "Audio service configuration must be a JSON object.");
        return false;
    }

    config_ = config;
    speechPath_ = trimTrailingSlashes(stringValueOrDefault(config_, "speechPath", "speech_path", "/api/audio/speech"));
    transcriptionPath_ = trimTrailingSlashes(stringValueOrDefault(config_, "transcriptionPath", "transcription_path", "/api/audio/transcriptions"));
    voicesPath_ = trimTrailingSlashes(stringValueOrDefault(config_, "voicesPath", "voices_path", "/api/audio/voices"));
    config_["speechPath"] = speechPath_;
    config_["transcriptionPath"] = transcriptionPath_;
    config_["voicesPath"] = voicesPath_;

    if (!client_) {
        client_ = std::make_shared<BaseClient>();
    }

    if (!client_->isInitialized()) {
        if (!client_->initialize(config_)) {
            setLastError(client_->getLastErrorCode().empty() ? "client_initialization_failed" : client_->getLastErrorCode(),
                         client_->getLastErrorMessage().empty() ? "Audio service could not initialize its BaseClient." : client_->getLastErrorMessage());
            initialized_ = false;
            return false;
        }
    }

    initialized_ = true;
    clearLastError();
    return true;
}

void Audio::shutdown() {
    initialized_ = false;
    config_ = nlohmann::json::object();
    speechPath_ = "/api/audio/speech";
    transcriptionPath_ = "/api/audio/transcriptions";
    voicesPath_ = "/api/audio/voices";
    clearLastError();
}

void Audio::setClient(std::shared_ptr<BaseClient> client) {
    client_ = std::move(client);
}

ApiResult Audio::synthesizeSpeech(const nlohmann::json& request) const {
    if (!initialized_) {
        return ApiResult::failure("audio_not_initialized", "Audio service must be initialized before speech synthesis can be requested.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Audio service has no BaseClient configured.");
    }
    if (!request.is_object()) {
        return ApiResult::failure("invalid_audio_request", "Speech synthesis request must be a JSON object.");
    }
    if (!hasNonEmptyString(request, "text") && !hasNonEmptyString(request, "ssml")) {
        return ApiResult::failure("missing_speech_text", "Speech synthesis request requires non-empty text or ssml.");
    }
    return client_->request("POST", speechPath_, request);
}

ApiResult Audio::transcribeAudio(const nlohmann::json& request) const {
    if (!initialized_) {
        return ApiResult::failure("audio_not_initialized", "Audio service must be initialized before transcription can be requested.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Audio service has no BaseClient configured.");
    }
    if (!request.is_object()) {
        return ApiResult::failure("invalid_audio_request", "Transcription request must be a JSON object.");
    }
    if (!hasNonEmptyString(request, "audioUrl") &&
        !hasNonEmptyString(request, "audio_url") &&
        !hasNonEmptyString(request, "audioBase64") &&
        !hasNonEmptyString(request, "audio_base64") &&
        !hasNonEmptyString(request, "fileId") &&
        !hasNonEmptyString(request, "file_id")) {
        return ApiResult::failure("missing_audio_source", "Transcription request requires an audio URL, base64 payload, or file ID.");
    }
    return client_->request("POST", transcriptionPath_, request);
}

ApiResult Audio::listVoices() const {
    if (!initialized_) {
        return ApiResult::failure("audio_not_initialized", "Audio service must be initialized before voices can be listed.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Audio service has no BaseClient configured.");
    }
    return client_->request("GET", voicesPath_);
}

nlohmann::json Audio::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    status["speechPath"] = speechPath_;
    status["transcriptionPath"] = transcriptionPath_;
    status["voicesPath"] = voicesPath_;
    status["clientConfigured"] = static_cast<bool>(client_);
    status["client"] = client_ ? client_->getStatus() : nlohmann::json::object();
    status["lastErrorCode"] = lastErrorCode_;
    status["lastErrorMessage"] = lastErrorMessage_;
    return status;
}

void Audio::setLastError(std::string code, std::string message) {
    lastErrorCode_ = std::move(code);
    lastErrorMessage_ = std::move(message);
}

void Audio::clearLastError() {
    lastErrorCode_.clear();
    lastErrorMessage_.clear();
}

} // namespace eliza_api_client
} // namespace elizaos
