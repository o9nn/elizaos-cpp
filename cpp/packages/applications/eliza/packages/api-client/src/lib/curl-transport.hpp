#ifndef ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_LIB_CURL_TRANSPORT_HPP_
#define ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_LIB_CURL_TRANSPORT_HPP_

#include <map>
#include <memory>
#include <string>
#include <chrono>

#include <nlohmann/json.hpp>

#include "base-client.hpp"

namespace elizaos {
namespace eliza_api_client {

/**
 * Configuration options for the curl-based HTTP transport.
 */
struct CurlTransportConfig {
    int timeoutMs = 30000;              ///< Request timeout in milliseconds
    int connectTimeoutMs = 10000;       ///< Connection timeout in milliseconds
    int maxRetries = 3;                 ///< Maximum retry attempts
    int retryDelayMs = 1000;            ///< Delay between retries in milliseconds
    bool verifySSL = true;              ///< Whether to verify SSL certificates
    bool followRedirects = true;        ///< Whether to follow HTTP redirects
    int maxRedirects = 10;              ///< Maximum number of redirects to follow
    std::string userAgent = "ElizaOS-CPP/1.0";  ///< User agent string
    std::map<std::string, std::string> defaultHeaders;  ///< Default headers for all requests
};

/**
 * Production HTTP transport implementation using libcurl.
 * 
 * This transport provides:
 * - Full HTTP/HTTPS support
 * - Connection pooling via curl handle reuse
 * - Configurable timeouts and retry logic
 * - SSL/TLS verification
 * - Automatic redirect following
 * - JSON request/response handling
 * - Thread-safe operation (one transport per thread recommended)
 */
class CurlTransport : public IHttpTransport {
public:
    CurlTransport();
    explicit CurlTransport(const CurlTransportConfig& config);
    ~CurlTransport() override;

    // Non-copyable, but movable
    CurlTransport(const CurlTransport&) = delete;
    CurlTransport& operator=(const CurlTransport&) = delete;
    CurlTransport(CurlTransport&& other) noexcept;
    CurlTransport& operator=(CurlTransport&& other) noexcept;

    /**
     * Send an HTTP request and receive the response.
     * @param request The HTTP request to send.
     * @return ApiResult containing the response or error information.
     */
    ApiResult send(const HttpRequest& request) override;

    /**
     * Check if the transport is ready to send requests.
     * @return true if initialized successfully, false otherwise.
     */
    bool isReady() const override;

    /**
     * Get the transport name.
     * @return "curl_transport"
     */
    std::string getName() const override { return "curl_transport"; }

    /**
     * Get the current configuration.
     */
    const CurlTransportConfig& getConfig() const { return config_; }

    /**
     * Get diagnostic information about the transport.
     */
    nlohmann::json getDiagnostics() const;

    /**
     * Get statistics about requests made by this transport.
     */
    nlohmann::json getStats() const;

    /**
     * Reset statistics counters.
     */
    void resetStats();

private:
    bool initializeCurl();
    void cleanupCurl();
    std::string buildQueryString(const nlohmann::json& query) const;
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);
    static size_t headerCallback(char* buffer, size_t size, size_t nitems, void* userdata);
    ApiResult performRequest(const HttpRequest& request);
    ApiResult parseResponse(const std::string& responseBody, 
                           const std::map<std::string, std::string>& responseHeaders,
                           long httpCode) const;
    bool shouldRetry(long httpCode, int attemptNumber) const;

    CurlTransportConfig config_;
    void* curlHandle_ = nullptr;  // CURL* but using void* to avoid including curl.h
    bool initialized_ = false;

    // Statistics
    mutable int totalRequests_ = 0;
    mutable int successfulRequests_ = 0;
    mutable int failedRequests_ = 0;
    mutable int retriedRequests_ = 0;
    mutable std::chrono::milliseconds totalRequestTime_{0};
    mutable std::chrono::system_clock::time_point lastRequestTime_;
};

/**
 * Factory function to create a CurlTransport with default configuration.
 */
std::shared_ptr<IHttpTransport> createCurlTransport();

/**
 * Factory function to create a CurlTransport with custom configuration.
 */
std::shared_ptr<IHttpTransport> createCurlTransport(const CurlTransportConfig& config);

} // namespace eliza_api_client
} // namespace elizaos

#endif // ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_LIB_CURL_TRANSPORT_HPP_
