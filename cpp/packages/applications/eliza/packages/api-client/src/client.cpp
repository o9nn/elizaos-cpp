#include "client.hpp"

#include <stdexcept>

namespace elizaos {
namespace eliza_api_client {
namespace {

constexpr int kDefaultTimeoutMs = 30000;

bool startsWith(const std::string& value, const std::string& prefix) {
    return value.size() >= prefix.size() &&
           value.compare(0, prefix.size(), prefix) == 0;
}

bool isHttpUrl(const std::string& value) {
    return startsWith(value, "http://") || startsWith(value, "https://");
}

std::string stringFromKey(const nlohmann::json& config,
                          const std::vector<std::string>& keys) {
    for (const auto& key : keys) {
        auto it = config.find(key);
        if (it != config.end() && it->is_string()) {
            return it->get<std::string>();
        }
    }
    return {};
}

int timeoutFromConfig(const nlohmann::json& config) {
    for (const auto& key : {"timeoutMs", "timeout_ms"}) {
        auto it = config.find(key);
        if (it == config.end() || it->is_null()) {
            continue;
        }
        if (!it->is_number_integer()) {
            throw std::invalid_argument("API client timeout must be an integer number of milliseconds");
        }
        int value = it->get<int>();
        if (value <= 0) {
            throw std::invalid_argument("API client timeout must be positive");
        }
        return value;
    }
    return kDefaultTimeoutMs;
}

std::string normalizeBaseUrl(std::string base_url) {
    while (!base_url.empty() && base_url.back() == '/') {
        base_url.pop_back();
    }
    if (base_url.empty()) {
        throw std::invalid_argument("API client baseUrl must not be empty");
    }
    if (!isHttpUrl(base_url)) {
        throw std::invalid_argument("API client baseUrl must start with http:// or https://");
    }
    return base_url;
}

std::map<std::string, std::string> headersFromConfig(const nlohmann::json& config) {
    std::map<std::string, std::string> headers;
    auto it = config.find("headers");
    if (it == config.end() || !it->is_object()) {
        return headers;
    }

    for (auto header = it->begin(); header != it->end(); ++header) {
        if (header.value().is_string()) {
            headers.emplace(header.key(), header.value().get<std::string>());
        }
    }
    return headers;
}

std::string toLowerAscii(std::string value) {
    for (char& c : value) {
        if (c >= 'A' && c <= 'Z') {
            c = static_cast<char>(c - 'A' + 'a');
        }
    }
    return value;
}

bool isSensitiveHeaderName(const std::string& header_name) {
    const std::string lower = toLowerAscii(header_name);
    return lower == "authorization" ||
           lower == "proxy-authorization" ||
           lower == "x-api-key" ||
           lower == "api-key" ||
           lower == "apikey" ||
           lower.find("token") != std::string::npos ||
           lower.find("secret") != std::string::npos;
}

std::map<std::string, std::string> redactHeadersForStatus(
    const std::map<std::string, std::string>& headers) {
    std::map<std::string, std::string> redacted;
    for (const auto& [name, value] : headers) {
        redacted[name] = isSensitiveHeaderName(name) ? std::string("[REDACTED]") : value;
    }
    return redacted;
}

} // namespace

bool Client::initialize(const nlohmann::json& config) {
    if (initialized_) return true;

    try {
        const std::string configured_base_url =
            stringFromKey(config, {"baseUrl", "base_url"});
        if (configured_base_url.empty()) {
            setLastError("missing_base_url", "API client requires baseUrl or base_url");
            throw std::invalid_argument("API client requires baseUrl or base_url");
        }

        const std::string normalized_base_url = normalizeBaseUrl(configured_base_url);
        const int configured_timeout_ms = timeoutFromConfig(config);
        const std::string configured_api_key =
            stringFromKey(config, {"apiKey", "api_key"});
        const std::string configured_bearer_token =
            stringFromKey(config, {"bearerToken", "bearer_token"});
        const auto configured_headers = headersFromConfig(config);

        config_ = config;
        base_url_ = normalized_base_url;
        timeout_ms_ = configured_timeout_ms;
        api_key_ = configured_api_key;
        bearer_token_ = configured_bearer_token;
        default_headers_ = configured_headers;

        // Add authentication headers
        if (!api_key_.empty()) {
            default_headers_["X-API-Key"] = api_key_;
        }
        if (!bearer_token_.empty()) {
            default_headers_["Authorization"] = "Bearer " + bearer_token_;
        }

        // Create and initialize the shared BaseClient
        baseClient_ = std::make_shared<BaseClient>();
        
        nlohmann::json baseClientConfig;
        baseClientConfig["baseUrl"] = base_url_;
        baseClientConfig["timeoutMs"] = timeout_ms_;
        
        if (!baseClient_->initialize(baseClientConfig)) {
            setLastError(baseClient_->getLastErrorCode(), baseClient_->getLastErrorMessage());
            return false;
        }

        initialized_ = true;
        clearLastError();
        return true;
    } catch (const std::invalid_argument& e) {
        setLastError("invalid_configuration", e.what());
        throw;
    } catch (const std::exception& e) {
        setLastError("initialization_error", e.what());
        return false;
    }
}

void Client::shutdown() {
    // Shutdown services
    if (server_) {
        server_->shutdown();
        server_.reset();
    }
    if (system_) {
        system_->shutdown();
        system_.reset();
    }
    if (agents_) {
        agents_->shutdown();
        agents_.reset();
    }

    // Shutdown base client
    if (baseClient_) {
        baseClient_->shutdown();
        baseClient_.reset();
    }

    initialized_ = false;
    config_ = nlohmann::json::object();
    base_url_.clear();
    timeout_ms_ = kDefaultTimeoutMs;
    api_key_.clear();
    bearer_token_.clear();
    default_headers_.clear();
    clearLastError();
}

void Client::setTransport(std::shared_ptr<IHttpTransport> transport) {
    if (baseClient_) {
        baseClient_->setTransport(std::move(transport));
    }
}

nlohmann::json Client::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    status["baseUrl"] = initialized_ ? nlohmann::json(base_url_) : nlohmann::json(nullptr);
    status["timeoutMs"] = timeout_ms_;
    status["hasApiKey"] = hasApiKey();
    status["hasBearerToken"] = hasBearerToken();
    status["hasAuth"] = hasAuth();
    status["headerCount"] = default_headers_.size();
    status["defaultHeaders"] = redactHeadersForStatus(default_headers_);
    status["lastErrorCode"] = lastErrorCode_;
    status["lastErrorMessage"] = lastErrorMessage_;
    
    // Include base client status
    status["baseClient"] = baseClient_ ? baseClient_->getStatus() : nlohmann::json::object();
    
    // Include service statuses if initialized
    if (server_) {
        status["services"]["server"] = server_->getStatus();
    }
    if (system_) {
        status["services"]["system"] = system_->getStatus();
    }
    if (agents_) {
        status["services"]["agents"] = agents_->getStatus();
    }
    
    return status;
}

std::string Client::resolveEndpoint(const std::string& path) const {
    if (!initialized_) {
        throw std::logic_error("API client must be initialized before resolving endpoints");
    }
    if (path.empty() || path == "/") {
        return base_url_;
    }
    if (isHttpUrl(path)) {
        return path;
    }
    if (path.front() == '?') {
        return base_url_ + path;
    }
    if (path.front() == '/') {
        return base_url_ + path;
    }
    return base_url_ + "/" + path;
}

void Client::ensureServicesInitialized() const {
    if (!initialized_) {
        throw std::logic_error("Client must be initialized before accessing services");
    }
}

Server& Client::server() {
    ensureServicesInitialized();
    if (!server_) {
        server_ = std::make_unique<Server>(baseClient_);
        server_->initialize(config_);
    }
    return *server_;
}

const Server& Client::server() const {
    ensureServicesInitialized();
    if (!server_) {
        server_ = std::make_unique<Server>(baseClient_);
        server_->initialize(config_);
    }
    return *server_;
}

System& Client::system() {
    ensureServicesInitialized();
    if (!system_) {
        system_ = std::make_unique<System>(baseClient_);
        system_->initialize(config_);
    }
    return *system_;
}

const System& Client::system() const {
    ensureServicesInitialized();
    if (!system_) {
        system_ = std::make_unique<System>(baseClient_);
        system_->initialize(config_);
    }
    return *system_;
}

Agents& Client::agents() {
    ensureServicesInitialized();
    if (!agents_) {
        agents_ = std::make_unique<Agents>(baseClient_);
        agents_->initialize(config_);
    }
    return *agents_;
}

const Agents& Client::agents() const {
    ensureServicesInitialized();
    if (!agents_) {
        agents_ = std::make_unique<Agents>(baseClient_);
        agents_->initialize(config_);
    }
    return *agents_;
}

void Client::setLastError(std::string code, std::string message) {
    lastErrorCode_ = std::move(code);
    lastErrorMessage_ = std::move(message);
}

void Client::clearLastError() {
    lastErrorCode_.clear();
    lastErrorMessage_.clear();
}

} // namespace eliza_api_client
} // namespace elizaos
