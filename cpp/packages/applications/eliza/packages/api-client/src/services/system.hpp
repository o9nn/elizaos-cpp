#ifndef ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_SYSTEM_HPP_
#define ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_SYSTEM_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

#include "../lib/base-client.hpp"

namespace elizaos {
namespace eliza_api_client {

/**
 * System service for querying system-level information and configuration.
 * 
 * This service provides endpoints for:
 * - System configuration
 * - Runtime information
 * - System metrics
 */
class System {
public:
    System() = default;
    explicit System(std::shared_ptr<BaseClient> client);
    ~System() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();

    void setClient(std::shared_ptr<BaseClient> client);
    std::shared_ptr<BaseClient> getClient() const { return client_; }

    /**
     * Get system configuration.
     */
    ApiResult getConfig() const;

    /**
     * Get system runtime information (uptime, version, etc.).
     */
    ApiResult getInfo() const;

    /**
     * Get system metrics (memory usage, active connections, etc.).
     */
    ApiResult getMetrics() const;

    nlohmann::json getStatus() const;
    std::string getName() const { return "system"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getServiceConfig() const { return config_; }
    const std::string& getLastErrorCode() const { return lastErrorCode_; }
    const std::string& getLastErrorMessage() const { return lastErrorMessage_; }

private:
    void setLastError(std::string code, std::string message);
    void clearLastError();

    nlohmann::json config_ = nlohmann::json::object();
    bool initialized_ = false;
    std::string configPath_ = "/api/system/config";
    std::string infoPath_ = "/api/system/info";
    std::string metricsPath_ = "/api/system/metrics";
    std::string lastErrorCode_;
    std::string lastErrorMessage_;
    std::shared_ptr<BaseClient> client_;
};

} // namespace eliza_api_client
} // namespace elizaos

#endif // ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_SYSTEM_HPP_
