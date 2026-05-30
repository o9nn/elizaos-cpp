#include "agents.hpp"

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
} // namespace

Agents::Agents(std::shared_ptr<BaseClient> client)
    : client_(std::move(client)) {}

bool Agents::initialize(const nlohmann::json& config) {
    if (initialized_) {
        return true;
    }

    if (!config.is_object()) {
        setLastError("invalid_config", "Agents service configuration must be a JSON object.");
        return false;
    }

    config_ = config;
    basePath_ = stringValueOrDefault(config_, "basePath", "base_path", "/api/agents");
    config_["basePath"] = basePath_;

    if (!client_) {
        client_ = std::make_shared<BaseClient>();
    }

    if (!client_->isInitialized()) {
        if (!client_->initialize(config_)) {
            setLastError(client_->getLastErrorCode().empty() ? "client_initialization_failed" : client_->getLastErrorCode(),
                         client_->getLastErrorMessage().empty() ? "Agents service could not initialize its BaseClient." : client_->getLastErrorMessage());
            initialized_ = false;
            return false;
        }
    }

    initialized_ = true;
    clearLastError();
    return true;
}

void Agents::shutdown() {
    initialized_ = false;
    config_ = nlohmann::json::object();
    basePath_ = "/api/agents";
    clearLastError();
}

void Agents::setClient(std::shared_ptr<BaseClient> client) {
    client_ = std::move(client);
}

ApiResult Agents::list() const {
    if (!initialized_) {
        return ApiResult::failure("agents_not_initialized", "Agents service must be initialized before listing agents.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Agents service has no BaseClient configured.");
    }
    return client_->request("GET", basePath_);
}

ApiResult Agents::get(const std::string& agentId) const {
    if (!initialized_) {
        return ApiResult::failure("agents_not_initialized", "Agents service must be initialized before getting agent details.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Agents service has no BaseClient configured.");
    }
    if (agentId.empty()) {
        return ApiResult::failure("invalid_agent_id", "Agent ID must not be empty.");
    }
    return client_->request("GET", basePath_ + "/" + agentId);
}

ApiResult Agents::create(const nlohmann::json& agentConfig) const {
    if (!initialized_) {
        return ApiResult::failure("agents_not_initialized", "Agents service must be initialized before creating agents.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Agents service has no BaseClient configured.");
    }
    if (!agentConfig.is_object()) {
        return ApiResult::failure("invalid_config", "Agent configuration must be a JSON object.");
    }
    return client_->request("POST", basePath_, agentConfig);
}

ApiResult Agents::update(const std::string& agentId, const nlohmann::json& agentConfig) const {
    if (!initialized_) {
        return ApiResult::failure("agents_not_initialized", "Agents service must be initialized before updating agents.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Agents service has no BaseClient configured.");
    }
    if (agentId.empty()) {
        return ApiResult::failure("invalid_agent_id", "Agent ID must not be empty.");
    }
    if (!agentConfig.is_object()) {
        return ApiResult::failure("invalid_config", "Agent configuration must be a JSON object.");
    }
    return client_->request("PUT", basePath_ + "/" + agentId, agentConfig);
}

ApiResult Agents::remove(const std::string& agentId) const {
    if (!initialized_) {
        return ApiResult::failure("agents_not_initialized", "Agents service must be initialized before removing agents.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Agents service has no BaseClient configured.");
    }
    if (agentId.empty()) {
        return ApiResult::failure("invalid_agent_id", "Agent ID must not be empty.");
    }
    return client_->request("DELETE", basePath_ + "/" + agentId);
}

ApiResult Agents::getAgentStatus(const std::string& agentId) const {
    if (!initialized_) {
        return ApiResult::failure("agents_not_initialized", "Agents service must be initialized before getting agent status.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Agents service has no BaseClient configured.");
    }
    if (agentId.empty()) {
        return ApiResult::failure("invalid_agent_id", "Agent ID must not be empty.");
    }
    return client_->request("GET", basePath_ + "/" + agentId + "/status");
}

ApiResult Agents::start(const std::string& agentId) const {
    if (!initialized_) {
        return ApiResult::failure("agents_not_initialized", "Agents service must be initialized before starting agents.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Agents service has no BaseClient configured.");
    }
    if (agentId.empty()) {
        return ApiResult::failure("invalid_agent_id", "Agent ID must not be empty.");
    }
    return client_->request("POST", basePath_ + "/" + agentId + "/start");
}

ApiResult Agents::stop(const std::string& agentId) const {
    if (!initialized_) {
        return ApiResult::failure("agents_not_initialized", "Agents service must be initialized before stopping agents.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Agents service has no BaseClient configured.");
    }
    if (agentId.empty()) {
        return ApiResult::failure("invalid_agent_id", "Agent ID must not be empty.");
    }
    return client_->request("POST", basePath_ + "/" + agentId + "/stop");
}

nlohmann::json Agents::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    status["basePath"] = basePath_;
    status["clientConfigured"] = static_cast<bool>(client_);
    status["client"] = client_ ? client_->getStatus() : nlohmann::json::object();
    status["lastErrorCode"] = lastErrorCode_;
    status["lastErrorMessage"] = lastErrorMessage_;
    return status;
}

void Agents::setLastError(std::string code, std::string message) {
    lastErrorCode_ = std::move(code);
    lastErrorMessage_ = std::move(message);
}

void Agents::clearLastError() {
    lastErrorCode_.clear();
    lastErrorMessage_.clear();
}

} // namespace eliza_api_client
} // namespace elizaos
