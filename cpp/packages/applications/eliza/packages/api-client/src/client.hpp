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

namespace elizaos {
namespace eliza_api_client {

/**
 * Top-level API client aggregator that provides access to all ElizaOS API services.
 * 
 * The Client class:
 * - Manages a shared BaseClient that all services use
 * - Provides lazy-initialized service accessors
 * - Handles authentication and default headers
 * - Supports both API key and ****** authentication
 * 
 * Usage:
 *   Client client;
 *   client.initialize({{"baseUrl", "http://localhost:3000"}});
 *   auto health = client.server().getHealth();
 *   auto info = client.system().getInfo();
 */
class Client {
public:
    Client() = default;
    ~Client() = default;

    /**
     * Initialize the client with configuration.
     * Required config keys:
     *   - baseUrl or base_url: The API server base URL
     * Optional config keys:
     *   - apiKey or api_key: API key for authentication
     *   - bearerToken or bearer_token: ****** for authentication
     *   - timeoutMs or timeout_ms: Request timeout in milliseconds (default: 30000)
     *   - headers: Object with default headers to include in all requests
     */
    bool initialize(const nlohmann::json& config = {});
    
    /**
     * Shutdown the client and all services.
     */
    void shutdown();
    
    /**
     * Get the status of the client and all services.
     */
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
    const std::map<std::string, std::string>& getDefaultHeaders() const {
        return default_headers_;
    }

    std::string resolveEndpoint(const std::string& path) const;

    /**
     * Get the shared BaseClient instance.
     */
    std::shared_ptr<BaseClient> getBaseClient() const { return baseClient_; }

    /**
     * Set a custom HTTP transport for the shared BaseClient.
     */
    void setTransport(std::shared_ptr<IHttpTransport> transport);

    /**
     * Get the Server service for health and status endpoints.
     */
    Server& server();
    const Server& server() const;

    /**
     * Get the System service for configuration and metrics endpoints.
     */
    System& system();
    const System& system() const;

    /**
     * Get the Agents service for agent management endpoints.
     */
    Agents& agents();
    const Agents& agents() const;

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
};

} // namespace eliza_api_client
} // namespace elizaos

#endif // ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_CLIENT_HPP_
