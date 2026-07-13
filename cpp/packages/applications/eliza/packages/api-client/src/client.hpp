#ifndef ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_CLIENT_HPP_
#define ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_CLIENT_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

#include "lib/base-client.hpp"
#include "services/server.hpp"
#include "services/system.hpp"
#include "services/agents.hpp"
#include "services/audio.hpp"
#include "services/media.hpp"
#include "services/memory.hpp"
#include "services/messaging.hpp"

namespace elizaos {
namespace eliza_api_client {

/**
 * Top-level API client aggregator that provides access to all ElizaOS API services.
 *
 * The Client class manages one shared BaseClient and lazily initializes every
 * service over the same configured request boundary, ensuring endpoint services
 * share auth, base URL, timeout, and injected transports.
 */
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
    const std::string& getLastErrorCode() const { return lastErrorCode_; }
    const std::string& getLastErrorMessage() const { return lastErrorMessage_; }

    const std::string& getBaseUrl() const { return base_url_; }
    int getTimeoutMs() const { return timeout_ms_; }
    bool hasApiKey() const { return !api_key_.empty(); }
    bool hasBearerToken() const { return !bearer_token_.empty(); }
    bool hasAuth() const { return hasApiKey() || hasBearerToken(); }
    const std::map<std::string, std::string>& getDefaultHeaders() const { return default_headers_; }

    std::string resolveEndpoint(const std::string& path) const;
    std::shared_ptr<BaseClient> getBaseClient() const { return baseClient_; }
    void setTransport(std::shared_ptr<IHttpTransport> transport);

    Server& server();
    const Server& server() const;
    System& system();
    const System& system() const;
    Agents& agents();
    const Agents& agents() const;
    Audio& audio();
    const Audio& audio() const;
    Media& media();
    const Media& media() const;
    Memory& memory();
    const Memory& memory() const;
    Messaging& messaging();
    const Messaging& messaging() const;

private:
    void setLastError(std::string code, std::string message);
    void clearLastError();
    void ensureServicesInitialized() const;

    nlohmann::json config_ = nlohmann::json::object();
    bool initialized_ = false;
    std::string base_url_;
    int timeout_ms_ = 30000;
    std::string api_key_;
    std::string bearer_token_;
    std::map<std::string, std::string> default_headers_;
    std::string lastErrorCode_;
    std::string lastErrorMessage_;

    std::shared_ptr<BaseClient> baseClient_;
    mutable std::unique_ptr<Server> server_;
    mutable std::unique_ptr<System> system_;
    mutable std::unique_ptr<Agents> agents_;
    mutable std::unique_ptr<Audio> audio_;
    mutable std::unique_ptr<Media> media_;
    mutable std::unique_ptr<Memory> memory_;
    mutable std::unique_ptr<Messaging> messaging_;
};

} // namespace eliza_api_client
} // namespace elizaos

#endif // ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_CLIENT_HPP_
