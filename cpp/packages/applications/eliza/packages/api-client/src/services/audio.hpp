#ifndef ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_AUDIO_HPP_
#define ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_AUDIO_HPP_

#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "../lib/base-client.hpp"

namespace elizaos {
namespace eliza_api_client {

/**
 * Audio service for speech synthesis, transcription, and voice discovery.
 *
 * The service is deliberately transport-agnostic: it validates request shape,
 * resolves configurable endpoint paths, and delegates actual HTTP dispatch to
 * the shared BaseClient used by the top-level Client aggregator.
 */
class Audio {
public:
    Audio() = default;
    explicit Audio(std::shared_ptr<BaseClient> client);
    ~Audio() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();

    void setClient(std::shared_ptr<BaseClient> client);
    std::shared_ptr<BaseClient> getClient() const { return client_; }

    /**
     * Synthesize speech from a request object, typically containing text,
     * voice/model options, and output format preferences.
     */
    ApiResult synthesizeSpeech(const nlohmann::json& request) const;

    /**
     * Transcribe audio from a request object, typically containing a URL,
     * base64 payload, or storage reference plus language hints.
     */
    ApiResult transcribeAudio(const nlohmann::json& request) const;

    /**
     * List voices/models available from the backing ElizaOS audio service.
     */
    ApiResult listVoices() const;

    nlohmann::json getStatus() const;
    std::string getName() const { return "audio"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }
    const std::string& getLastErrorCode() const { return lastErrorCode_; }
    const std::string& getLastErrorMessage() const { return lastErrorMessage_; }

private:
    void setLastError(std::string code, std::string message);
    void clearLastError();

    nlohmann::json config_ = nlohmann::json::object();
    bool initialized_ = false;
    std::string speechPath_ = "/api/audio/speech";
    std::string transcriptionPath_ = "/api/audio/transcriptions";
    std::string voicesPath_ = "/api/audio/voices";
    std::string lastErrorCode_;
    std::string lastErrorMessage_;
    std::shared_ptr<BaseClient> client_;
};

} // namespace eliza_api_client
} // namespace elizaos

#endif // ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_AUDIO_HPP_
