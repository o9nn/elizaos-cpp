#include "system.hpp"

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

System::System(std::shared_ptr<BaseClient> client)
    : client_(std::move(client)) {}

bool System::initialize(const nlohmann::json& config) {
    if (initialized_) {
        return true;
    }

    if (!config.is_object()) {
        setLastError("invalid_config", "System service configuration must be a JSON object.");
        return false;
    }

    config_ = config;
    configPath_ = stringValueOrDefault(config_, "configPath", "config_path", "/api/system/config");
    infoPath_ = stringValueOrDefault(config_, "infoPath", "info_path", "/api/system/info");
    metricsPath_ = stringValueOrDefault(config_, "metricsPath", "metrics_path", "/api/system/metrics");
    config_["configPath"] = configPath_;
    config_["infoPath"] = infoPath_;
    config_["metricsPath"] = metricsPath_;

    if (!client_) {
        client_ = std::make_shared<BaseClient>();
    }

    if (!client_->isInitialized()) {
        if (!client_->initialize(config_)) {
            setLastError(client_->getLastErrorCode().empty() ? "client_initialization_failed" : client_->getLastErrorCode(),
                         client_->getLastErrorMessage().empty() ? "System service could not initialize its BaseClient." : client_->getLastErrorMessage());
            initialized_ = false;
            return false;
        }
    }

    initialized_ = true;
    clearLastError();
    return true;
}

void System::shutdown() {
    initialized_ = false;
    config_ = nlohmann::json::object();
    configPath_ = "/api/system/config";
    infoPath_ = "/api/system/info";
    metricsPath_ = "/api/system/metrics";
    clearLastError();
}

void System::setClient(std::shared_ptr<BaseClient> client) {
    client_ = std::move(client);
}

ApiResult System::getConfig() const {
    if (!initialized_) {
        return ApiResult::failure("system_not_initialized", "System service must be initialized before config can be requested.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "System service has no BaseClient configured.");
    }
    return client_->request("GET", configPath_);
}

ApiResult System::getInfo() const {
    if (!initialized_) {
        return ApiResult::failure("system_not_initialized", "System service must be initialized before info can be requested.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "System service has no BaseClient configured.");
    }
    return client_->request("GET", infoPath_);
}

ApiResult System::getMetrics() const {
    if (!initialized_) {
        return ApiResult::failure("system_not_initialized", "System service must be initialized before metrics can be requested.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "System service has no BaseClient configured.");
    }
    return client_->request("GET", metricsPath_);
}

nlohmann::json System::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    status["configPath"] = configPath_;
    status["infoPath"] = infoPath_;
    status["metricsPath"] = metricsPath_;
    status["clientConfigured"] = static_cast<bool>(client_);
    status["client"] = client_ ? client_->getStatus() : nlohmann::json::object();
    status["lastErrorCode"] = lastErrorCode_;
    status["lastErrorMessage"] = lastErrorMessage_;
    return status;
}

void System::setLastError(std::string code, std::string message) {
    lastErrorCode_ = std::move(code);
    lastErrorMessage_ = std::move(message);
}

void System::clearLastError() {
    lastErrorCode_.clear();
    lastErrorMessage_.clear();
}

} // namespace eliza_api_client
} // namespace elizaos
