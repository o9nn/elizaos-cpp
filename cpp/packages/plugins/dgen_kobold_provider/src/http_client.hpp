#pragma once

#include <chrono>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

namespace elizaos {

enum class HttpMethod { Get, Post };
enum class RetryMode { Never };

struct RetryPolicy {
    RetryMode mode{RetryMode::Never};
    std::size_t max_attempts{1U};
};

using HttpHeaders = std::vector<std::pair<std::string, std::string>>;

struct HttpRequest {
    HttpMethod method{HttpMethod::Get};
    std::string url;
    HttpHeaders headers;
    std::string body;
    std::chrono::milliseconds connect_timeout{5000};
    std::chrono::milliseconds request_timeout{60000};
    std::size_t max_response_bytes{2U * 1024U * 1024U};
    bool idempotent{false};
    RetryPolicy retry_policy{};
};

struct HttpResponse {
    long status_code{0};
    std::string content_type;
    std::string body;
};

enum class TransportErrorCode { Unavailable, Timeout, ResponseTooLarge, Network };

class TransportError final : public std::runtime_error {
public:
    explicit TransportError(TransportErrorCode code);
    TransportErrorCode code() const noexcept;

private:
    TransportErrorCode code_;
};

/** Concurrent implementations must not log requests or retry automatically. */
class HttpTransport {
public:
    virtual ~HttpTransport() = default;
    virtual HttpResponse execute(const HttpRequest& request) const = 0;
};

struct HttpRequestOptions {
    std::chrono::milliseconds connect_timeout{5000};
    std::chrono::milliseconds request_timeout{60000};
    std::size_t max_response_bytes{2U * 1024U * 1024U};
};

/**
 * JSON client over an explicitly supplied transport. Credentials and bodies are
 * never retained. Production callers must explicitly request the libcurl adapter.
 */
class HttpClient {
public:
    explicit HttpClient(std::shared_ptr<const HttpTransport> transport,
                        HttpRequestOptions options = {});

    nlohmann::json postJson(const std::string& url,
                            const nlohmann::json& body,
                            const std::string& bearer_token = "") const;
    nlohmann::json getJson(const std::string& url) const;

    static bool libcurlAvailable() noexcept;
    static std::shared_ptr<const HttpTransport> createLibcurlTransport();

private:
    nlohmann::json executeJson(HttpMethod method,
                               const std::string& url,
                               const std::string& body,
                               const std::string& bearer_token,
                               bool idempotent) const;

    std::shared_ptr<const HttpTransport> transport_;
    const HttpRequestOptions options_;
};

} // namespace elizaos
