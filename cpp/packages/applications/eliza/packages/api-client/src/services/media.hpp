#ifndef ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_MEDIA_HPP_
#define ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_MEDIA_HPP_

#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "../lib/base-client.hpp"

namespace elizaos {
namespace eliza_api_client {

class Media {
public:
    Media() = default;
    explicit Media(std::shared_ptr<BaseClient> client);
    ~Media() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    void setClient(std::shared_ptr<BaseClient> client);
    std::shared_ptr<BaseClient> getClient() const { return client_; }

    ApiResult uploadMedia(const nlohmann::json& descriptor) const;
    ApiResult listMedia() const;
    ApiResult getMedia(const std::string& mediaId) const;
    ApiResult deleteMedia(const std::string& mediaId) const;

    nlohmann::json getStatus() const;
    std::string getName() const { return "media"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }
    const std::string& getLastErrorCode() const { return lastErrorCode_; }
    const std::string& getLastErrorMessage() const { return lastErrorMessage_; }

private:
    void setLastError(std::string code, std::string message);
    void clearLastError();
    std::string mediaPath(const std::string& mediaId) const;

    nlohmann::json config_ = nlohmann::json::object();
    bool initialized_ = false;
    std::string basePath_ = "/api/media";
    std::string lastErrorCode_;
    std::string lastErrorMessage_;
    std::shared_ptr<BaseClient> client_;
};

} // namespace eliza_api_client
} // namespace elizaos

#endif // ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_MEDIA_HPP_
