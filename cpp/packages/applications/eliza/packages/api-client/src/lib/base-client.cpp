#include "base-client.hpp"

#include <algorithm>
#include <cctype>
#include <utility>

namespace elizaos {
namespace eliza_api_client {

ApiResult ApiResult::success(HttpResponse response) {
    ApiResult result;
    result.ok = true;
    result.response = std::move(response);
    return result;
}

ApiResult ApiResult::failure(std::string code, std::string message, HttpResponse response) {
    ApiResult result;
    result.ok = false;
    result.response = std::move(response);
    result.errorCode = std::move(code);
    result.errorMessage = std::move(message);
    return result;
}

BaseClient::BaseClient(std::shared_ptr<IHttpTransport> transport)
    : transport_(std::move(transport)) {}

bool BaseClient::initialize(const nlohmann::json& config) {
    if (!validateConfig(config)) {
        initialized_ = false;
        configured_ = false;
        config_ = nlohmann::json::object();
        baseUrl_.clear();
        return false;
    }

    config_ = config;
    if (!config_.contains("baseUrl") && config_.contains("base_url")) {
        config_["baseUrl"] = config_.at("base_url");
    }
    config_["baseUrl"] = baseUrl_;
    config_["timeoutMs"] = timeoutMs_;
    config_["maxRetries"] = maxRetries_;

    configured_ = true;
    initialized_ = true;
    clearLastError();
    return true;
}

void BaseClient::shutdown() {
    initialized_ = false;
    configured_ = false;
    config_ = nlohmann::json::object();
    baseUrl_.clear();
    timeoutMs_ = 30000;
    maxRetries_ = 0;
    clearLastError();
}

void BaseClient::setTransport(std::shared_ptr<IHttpTransport> transport) {
    transport_ = std::move(transport);
}

ApiResult BaseClient::request(const std::string& method,
                              const std::string& path,
                              const nlohmann::json& body,
                              const std::map<std::string, std::string>& headers) const {
    if (!initialized_ || !configured_) {
        return ApiResult::failure("client_not_initialized", "BaseClient must be successfully initialized before requests can be sent.");
    }
    if (!transport_) {
        return ApiResult::failure("transport_missing", "No HTTP transport has been configured for BaseClient.");
    }
    if (!transport_->isReady()) {
        return ApiResult::failure("transport_not_ready", "The configured HTTP transport reports that it is not ready.");
    }

    HttpRequest request;
    request.method = method;
    request.path = path;
    request.url = buildUrl(path);
    request.body = body;
    request.headers = headers;
    return transport_->send(request);
}

std::string BaseClient::buildUrl(const std::string& path) const {
    if (baseUrl_.empty()) {
        return path;
    }
    if (path.empty()) {
        return baseUrl_;
    }
    if (path.rfind("http://", 0) == 0 || path.rfind("https://", 0) == 0) {
        return path;
    }
    if (path.front() == '/') {
        return baseUrl_ + path;
    }
    return baseUrl_ + "/" + path;
}

nlohmann::json BaseClient::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    status["configured"] = configured_;
    status["baseUrl"] = baseUrl_;
    status["timeoutMs"] = timeoutMs_;
    status["maxRetries"] = maxRetries_;
    status["transportConfigured"] = static_cast<bool>(transport_);
    status["transportReady"] = isTransportReady();
    status["transportName"] = transport_ ? transport_->getName() : "none";
    status["lastErrorCode"] = lastErrorCode_;
    status["lastErrorMessage"] = lastErrorMessage_;
    return status;
}

bool BaseClient::isTransportReady() const {
    return transport_ && transport_->isReady();
}

std::string BaseClient::trim(const std::string& value) {
    auto begin = std::find_if_not(value.begin(), value.end(), [](unsigned char ch) { return std::isspace(ch) != 0; });
    auto end = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char ch) { return std::isspace(ch) != 0; }).base();
    if (begin >= end) {
        return {};
    }
    return std::string(begin, end);
}

std::string BaseClient::normalizeBaseUrl(const std::string& value) {
    std::string normalized = trim(value);
    while (normalized.size() > std::string("https://").size() && normalized.back() == '/') {
        normalized.pop_back();
    }
    return normalized;
}

bool BaseClient::isHttpUrl(const std::string& value) {
    return value.rfind("http://", 0) == 0 || value.rfind("https://", 0) == 0;
}

bool BaseClient::validateConfig(const nlohmann::json& config) {
    if (!config.is_object()) {
        setLastError("invalid_config", "BaseClient configuration must be a JSON object.");
        return false;
    }

    const auto baseUrlIt = config.find("baseUrl") != config.end() ? config.find("baseUrl") : config.find("base_url");
    if (baseUrlIt == config.end() || !baseUrlIt->is_string()) {
        setLastError("base_url_missing", "BaseClient requires a string baseUrl or base_url field.");
        return false;
    }

    const std::string normalizedBaseUrl = normalizeBaseUrl(baseUrlIt->get<std::string>());
    if (normalizedBaseUrl.empty()) {
        setLastError("base_url_empty", "BaseClient baseUrl must not be empty.");
        return false;
    }
    if (!isHttpUrl(normalizedBaseUrl)) {
        setLastError("base_url_invalid", "BaseClient baseUrl must start with http:// or https://.");
        return false;
    }

    int timeoutMs = config.value("timeoutMs", config.value("timeout_ms", 30000));
    if (timeoutMs <= 0) {
        setLastError("timeout_invalid", "BaseClient timeoutMs must be greater than zero.");
        return false;
    }

    int maxRetries = config.value("maxRetries", config.value("max_retries", 0));
    if (maxRetries < 0) {
        setLastError("retries_invalid", "BaseClient maxRetries must not be negative.");
        return false;
    }

    baseUrl_ = normalizedBaseUrl;
    timeoutMs_ = timeoutMs;
    maxRetries_ = maxRetries;
    return true;
}

void BaseClient::setLastError(std::string code, std::string message) {
    lastErrorCode_ = std::move(code);
    lastErrorMessage_ = std::move(message);
}

void BaseClient::clearLastError() {
    lastErrorCode_.clear();
    lastErrorMessage_.clear();
}

} // namespace eliza_api_client
} // namespace elizaos
