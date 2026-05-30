#ifndef ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_AGENTS_HPP_
#define ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_AGENTS_HPP_

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
 * Agents service for managing ElizaOS agents.
 * 
 * This service provides endpoints for:
 * - Listing all agents
 * - Getting agent details
 * - Creating new agents
 * - Updating agent configuration
 * - Deleting agents
 * - Agent status and health
 */
class Agents {
public:
    Agents() = default;
    explicit Agents(std::shared_ptr<BaseClient> client);
    ~Agents() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();

    void setClient(std::shared_ptr<BaseClient> client);
    std::shared_ptr<BaseClient> getClient() const { return client_; }

    /**
     * Get list of all agents.
     */
    ApiResult list() const;

    /**
     * Get details for a specific agent.
     * @param agentId The agent identifier.
     */
    ApiResult get(const std::string& agentId) const;

    /**
     * Create a new agent.
     * @param agentConfig The agent configuration.
     */
    ApiResult create(const nlohmann::json& agentConfig) const;

    /**
     * Update an existing agent.
     * @param agentId The agent identifier.
     * @param agentConfig The updated agent configuration.
     */
    ApiResult update(const std::string& agentId, const nlohmann::json& agentConfig) const;

    /**
     * Delete an agent.
     * @param agentId The agent identifier.
     */
    ApiResult remove(const std::string& agentId) const;

    /**
     * Get the status of a specific agent.
     * @param agentId The agent identifier.
     */
    ApiResult getAgentStatus(const std::string& agentId) const;

    /**
     * Start an agent.
     * @param agentId The agent identifier.
     */
    ApiResult start(const std::string& agentId) const;

    /**
     * Stop an agent.
     * @param agentId The agent identifier.
     */
    ApiResult stop(const std::string& agentId) const;

    nlohmann::json getStatus() const;
    std::string getName() const { return "agents"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getServiceConfig() const { return config_; }
    const std::string& getLastErrorCode() const { return lastErrorCode_; }
    const std::string& getLastErrorMessage() const { return lastErrorMessage_; }

private:
    void setLastError(std::string code, std::string message);
    void clearLastError();

    nlohmann::json config_ = nlohmann::json::object();
    bool initialized_ = false;
    std::string basePath_ = "/api/agents";
    std::string lastErrorCode_;
    std::string lastErrorMessage_;
    std::shared_ptr<BaseClient> client_;
};

} // namespace eliza_api_client
} // namespace elizaos

#endif // ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_AGENTS_HPP_
