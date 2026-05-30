#ifndef ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_LIB_BASE_CLIENT_HPP_
#define ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_LIB_BASE_CLIENT_HPP_

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace elizaos {
namespace eliza_api_client {

struct HttpRequest {
    std::string method;
    std::string url;
    std::string path;
    std::map<std::string, std::string> headers;
    nlohmann::json query = nlohmann::json::object();
    nlohmann::json body = nlohmann::json::object();
};

struct HttpResponse {
    int statusCode = 0;
    std::map<std::string, std::string> headers;
    std::string body;
    nlohmann::json jsonBody = nlohmann::json::object();
};

struct ApiResult {
    bool ok = false;
    HttpResponse response;
    std::string errorCode;
    std::string errorMessage;

    static ApiResult success(HttpResponse response);
    static ApiResult failure(std::string code, std::string message, HttpResponse response = {});
};

class IHttpTransport {
public:
    virtual ~IHttpTransport() = default;
    virtual ApiResult send(const HttpRequest& request) = 0;
    virtual bool isReady() const { return true; }
    virtual std::string getName() const { return "http_transport"; }
};

class BaseClient {
public:
    BaseClient() = default;
    explicit BaseClient(std::shared_ptr<IHttpTransport> transport);
    ~BaseClient() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();

    void setTransport(std::shared_ptr<IHttpTransport> transport);
    std::shared_ptr<IHttpTransport> getTransport() const { return transport_; }

    ApiResult request(const std::string& method,
                      const std::string& path,
                      const nlohmann::json& body = nlohmann::json::object(),
                      const std::map<std::string, std::string>& headers = {}) const;

    std::string buildUrl(const std::string& path) const;
    nlohmann::json getStatus() const;
    std::string getName() const { return "base_client"; }
    bool isInitialized() const { return initialized_; }
    bool isConfigured() const { return configured_; }
    bool isTransportReady() const;
    const nlohmann::json& getConfig() const { return config_; }
    const std::string& getBaseUrl() const { return baseUrl_; }
    const std::string& getLastErrorCode() const { return lastErrorCode_; }
    const std::string& getLastErrorMessage() const { return lastErrorMessage_; }

private:
    static std::string trim(const std::string& value);
    static std::string normalizeBaseUrl(const std::string& value);
    static bool isHttpUrl(const std::string& value);
    bool validateConfig(const nlohmann::json& config);
    void setLastError(std::string code, std::string message);
    void clearLastError();

    nlohmann::json config_ = nlohmann::json::object();
    bool initialized_ = false;
    bool configured_ = false;
    std::string baseUrl_;
    int timeoutMs_ = 30000;
    int maxRetries_ = 0;
    std::string lastErrorCode_;
    std::string lastErrorMessage_;
    std::shared_ptr<IHttpTransport> transport_;
};

} // namespace eliza_api_client
} // namespace elizaos

#endif // ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_LIB_BASE_CLIENT_HPP_
