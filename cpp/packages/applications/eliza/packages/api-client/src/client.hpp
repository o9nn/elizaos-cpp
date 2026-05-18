#ifndef ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_CLIENT_HPP_
#define ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_CLIENT_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace eliza_api_client {

class Client {
public:
    Client() = default;
    ~Client() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "client"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

    const std::string& getBaseUrl() const { return base_url_; }
    int getTimeoutMs() const { return timeout_ms_; }
    bool hasApiKey() const { return !api_key_.empty(); }
    bool hasBearerToken() const { return !bearer_token_.empty(); }
    bool hasAuth() const { return hasApiKey() || hasBearerToken(); }
    const std::map<std::string, std::string>& getDefaultHeaders() const {
        return default_headers_;
    }

    std::string resolveEndpoint(const std::string& path) const;

private:
    nlohmann::json config_;
    bool initialized_ = false;
    std::string base_url_;
    int timeout_ms_ = 30000;
    std::string api_key_;
    std::string bearer_token_;
    std::map<std::string, std::string> default_headers_;
};

} // namespace eliza_api_client
} // namespace elizaos

#endif // ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_CLIENT_HPP_
