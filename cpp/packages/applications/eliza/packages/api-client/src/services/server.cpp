#include "server.hpp"

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

Server::Server(std::shared_ptr<BaseClient> client)
    : client_(std::move(client)) {}

bool Server::initialize(const nlohmann::json& config) {
    if (initialized_) {
        return true;
    }

    if (!config.is_object()) {
        setLastError("invalid_config", "Server service configuration must be a JSON object.");
        return false;
    }

    config_ = config;
    healthPath_ = stringValueOrDefault(config_, "healthPath", "health_path", "/api/server/health");
    statusPath_ = stringValueOrDefault(config_, "statusPath", "status_path", "/api/server/status");
    config_["healthPath"] = healthPath_;
    config_["statusPath"] = statusPath_;

    if (!client_) {
        client_ = std::make_shared<BaseClient>();
    }

    if (!client_->isInitialized()) {
        if (!client_->initialize(config_)) {
            setLastError(client_->getLastErrorCode().empty() ? "client_initialization_failed" : client_->getLastErrorCode(),
                         client_->getLastErrorMessage().empty() ? "Server service could not initialize its BaseClient." : client_->getLastErrorMessage());
            initialized_ = false;
            return false;
        }
    }

    initialized_ = true;
    clearLastError();
    return true;
}

void Server::shutdown() {
    initialized_ = false;
    config_ = nlohmann::json::object();
    healthPath_ = "/api/server/health";
    statusPath_ = "/api/server/status";
    clearLastError();
}

void Server::setClient(std::shared_ptr<BaseClient> client) {
    client_ = std::move(client);
}

ApiResult Server::getHealth() const {
    if (!initialized_) {
        return ApiResult::failure("server_not_initialized", "Server service must be initialized before health can be requested.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Server service has no BaseClient configured.");
    }
    return client_->request("GET", healthPath_);
}

ApiResult Server::getServerStatus() const {
    if (!initialized_) {
        return ApiResult::failure("server_not_initialized", "Server service must be initialized before server status can be requested.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Server service has no BaseClient configured.");
    }
    return client_->request("GET", statusPath_);
}

nlohmann::json Server::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    status["healthPath"] = healthPath_;
    status["statusPath"] = statusPath_;
    status["clientConfigured"] = static_cast<bool>(client_);
    status["client"] = client_ ? client_->getStatus() : nlohmann::json::object();
    status["lastErrorCode"] = lastErrorCode_;
    status["lastErrorMessage"] = lastErrorMessage_;
    return status;
}

void Server::setLastError(std::string code, std::string message) {
    lastErrorCode_ = std::move(code);
    lastErrorMessage_ = std::move(message);
}

void Server::clearLastError() {
    lastErrorCode_.clear();
    lastErrorMessage_.clear();
}

} // namespace eliza_api_client
} // namespace elizaos
