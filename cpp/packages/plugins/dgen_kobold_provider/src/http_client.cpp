#include "http_client.hpp"

#include <algorithm>
#include <cctype>
#include <limits>
#include <mutex>
#include <sstream>
#include <utility>

#ifndef ELIZAOS_DGEN_KOBOLD_HAS_LIBCURL
#define ELIZAOS_DGEN_KOBOLD_HAS_LIBCURL 0
#endif

#if ELIZAOS_DGEN_KOBOLD_HAS_LIBCURL
#include <curl/curl.h>
#endif

namespace elizaos {
namespace {

constexpr std::size_t kMaxRequestBytes = 512U * 1024U;
constexpr std::size_t kMaxResponseLimit = 16U * 1024U * 1024U;
constexpr auto kMaxConnectTimeout = std::chrono::seconds(30);
constexpr auto kMaxRequestTimeout = std::chrono::minutes(5);

const char* safeTransportMessage(TransportErrorCode code) noexcept {
    switch (code) {
    case TransportErrorCode::Unavailable: return "HTTP transport is unavailable";
    case TransportErrorCode::Timeout: return "HTTP request timed out";
    case TransportErrorCode::ResponseTooLarge:
        return "HTTP response exceeded the configured size limit";
    case TransportErrorCode::Network: return "HTTP transport failed";
    }
    return "HTTP transport failed";
}

void validateOptions(const HttpRequestOptions& options) {
    if (options.connect_timeout.count() <= 0 ||
        options.connect_timeout > kMaxConnectTimeout) {
        throw std::invalid_argument("HTTP connect timeout is outside the allowed range");
    }
    if (options.request_timeout.count() <= 0 ||
        options.request_timeout > kMaxRequestTimeout ||
        options.request_timeout < options.connect_timeout) {
        throw std::invalid_argument("HTTP request timeout is outside the allowed range");
    }
    if (options.max_response_bytes == 0U ||
        options.max_response_bytes > kMaxResponseLimit) {
        throw std::invalid_argument("HTTP response size limit is outside the allowed range");
    }
}

void validateUrl(const std::string& url) {
    if (url.empty() || url.size() > 2048U ||
        std::any_of(url.begin(), url.end(), [](unsigned char character) {
            return std::iscntrl(character) != 0 ||
                   std::isspace(character) != 0 || character == '\\';
        })) {
        throw std::invalid_argument("HTTP URL is invalid");
    }
}

void validateBearerToken(const std::string& token) {
    if (token.size() > 8192U ||
        std::any_of(token.begin(), token.end(), [](unsigned char character) {
            return character <= 0x20U || character == 0x7FU;
        })) {
        throw std::invalid_argument("Bearer credential is invalid");
    }
}

std::string mediaType(std::string content_type) {
    const std::size_t parameter = content_type.find(';');
    if (parameter != std::string::npos) {
        content_type.erase(parameter);
    }
    while (!content_type.empty() &&
           std::isspace(static_cast<unsigned char>(content_type.back())) != 0) {
        content_type.pop_back();
    }
    while (!content_type.empty() &&
           std::isspace(static_cast<unsigned char>(content_type.front())) != 0) {
        content_type.erase(content_type.begin());
    }
    std::transform(content_type.begin(), content_type.end(), content_type.begin(),
                   [](unsigned char character) {
                       return static_cast<char>(std::tolower(character));
                   });
    return content_type;
}

bool isJsonContentType(const std::string& value) {
    const std::string type = mediaType(value);
    constexpr const char suffix[] = "+json";
    return type == "application/json" ||
           (type.size() > sizeof(suffix) - 1U &&
            type.compare(type.size() - (sizeof(suffix) - 1U),
                         sizeof(suffix) - 1U, suffix) == 0);
}

#if ELIZAOS_DGEN_KOBOLD_HAS_LIBCURL

class CurlHandle final {
public:
    CurlHandle() : value_(curl_easy_init()) {}
    ~CurlHandle() { if (value_ != nullptr) curl_easy_cleanup(value_); }
    CurlHandle(const CurlHandle&) = delete;
    CurlHandle& operator=(const CurlHandle&) = delete;
    CURL* get() const noexcept { return value_; }
private:
    CURL* value_;
};

class CurlHeaders final {
public:
    CurlHeaders() = default;
    ~CurlHeaders() { if (value_ != nullptr) curl_slist_free_all(value_); }
    CurlHeaders(const CurlHeaders&) = delete;
    CurlHeaders& operator=(const CurlHeaders&) = delete;
    void append(const std::string& value) {
        curl_slist* appended = curl_slist_append(value_, value.c_str());
        if (appended == nullptr) throw TransportError(TransportErrorCode::Unavailable);
        value_ = appended;
    }
    curl_slist* get() const noexcept { return value_; }
private:
    curl_slist* value_{nullptr};
};

struct WriteContext {
    std::string body;
    std::size_t limit{0U};
    bool exceeded{false};
};

std::size_t writeCallback(char* data, std::size_t size, std::size_t count,
                          void* user_data) noexcept {
    auto* context = static_cast<WriteContext*>(user_data);
    if (context == nullptr ||
        (count != 0U && size > std::numeric_limits<std::size_t>::max() / count)) {
        return 0U;
    }
    const std::size_t incoming = size * count;
    const std::size_t current = std::min(context->limit, context->body.size());
    if (incoming > context->limit - current) {
        context->exceeded = true;
        return 0U;
    }
    try { context->body.append(data, incoming); } catch (...) { return 0U; }
    return incoming;
}

template <typename Value>
void setOption(CURL* handle, CURLoption option, Value value) {
    if (curl_easy_setopt(handle, option, value) != CURLE_OK) {
        throw TransportError(TransportErrorCode::Unavailable);
    }
}

void ensureCurlInitialized() {
    static std::once_flag once;
    static CURLcode result = CURLE_FAILED_INIT;
    std::call_once(once, [] { result = curl_global_init(CURL_GLOBAL_DEFAULT); });
    if (result != CURLE_OK) throw TransportError(TransportErrorCode::Unavailable);
}

class LibcurlTransport final : public HttpTransport {
public:
    HttpResponse execute(const HttpRequest& request) const override {
        ensureCurlInitialized();
        CurlHandle handle;
        if (handle.get() == nullptr) throw TransportError(TransportErrorCode::Unavailable);
        CurlHeaders headers;
        for (const auto& header : request.headers) {
            headers.append(header.first + ": " + header.second);
        }
        WriteContext context;
        context.limit = request.max_response_bytes;
        setOption(handle.get(), CURLOPT_URL, request.url.c_str());
        setOption(handle.get(), CURLOPT_HTTPHEADER, headers.get());
        setOption(handle.get(), CURLOPT_WRITEFUNCTION, &writeCallback);
        setOption(handle.get(), CURLOPT_WRITEDATA, &context);
        setOption(handle.get(), CURLOPT_CONNECTTIMEOUT_MS,
                  static_cast<long>(request.connect_timeout.count()));
        setOption(handle.get(), CURLOPT_TIMEOUT_MS,
                  static_cast<long>(request.request_timeout.count()));
        setOption(handle.get(), CURLOPT_NOSIGNAL, 1L);
        setOption(handle.get(), CURLOPT_FOLLOWLOCATION, 0L);
        setOption(handle.get(), CURLOPT_MAXREDIRS, 0L);
        setOption(handle.get(), CURLOPT_SSL_VERIFYPEER, 1L);
        setOption(handle.get(), CURLOPT_SSL_VERIFYHOST, 2L);
        setOption(handle.get(), CURLOPT_VERBOSE, 0L);
        setOption(handle.get(), CURLOPT_NOPROXY, "*");
#if LIBCURL_VERSION_NUM >= 0x075500
        setOption(handle.get(), CURLOPT_PROTOCOLS_STR, "http,https");
        setOption(handle.get(), CURLOPT_REDIR_PROTOCOLS_STR, "http,https");
#else
        setOption(handle.get(), CURLOPT_PROTOCOLS,
                  static_cast<long>(CURLPROTO_HTTP | CURLPROTO_HTTPS));
        setOption(handle.get(), CURLOPT_REDIR_PROTOCOLS,
                  static_cast<long>(CURLPROTO_HTTP | CURLPROTO_HTTPS));
#endif
        if (request.method == HttpMethod::Post) {
            setOption(handle.get(), CURLOPT_POST, 1L);
            setOption(handle.get(), CURLOPT_POSTFIELDS, request.body.c_str());
            setOption(handle.get(), CURLOPT_POSTFIELDSIZE_LARGE,
                      static_cast<curl_off_t>(request.body.size()));
        } else {
            setOption(handle.get(), CURLOPT_HTTPGET, 1L);
        }
        const CURLcode result = curl_easy_perform(handle.get());
        if (context.exceeded) throw TransportError(TransportErrorCode::ResponseTooLarge);
        if (result == CURLE_OPERATION_TIMEDOUT) {
            throw TransportError(TransportErrorCode::Timeout);
        }
        if (result != CURLE_OK) throw TransportError(TransportErrorCode::Network);
        HttpResponse response;
        if (curl_easy_getinfo(handle.get(), CURLINFO_RESPONSE_CODE,
                              &response.status_code) != CURLE_OK) {
            throw TransportError(TransportErrorCode::Network);
        }
        char* content_type = nullptr;
        if (curl_easy_getinfo(handle.get(), CURLINFO_CONTENT_TYPE,
                              &content_type) != CURLE_OK) {
            throw TransportError(TransportErrorCode::Network);
        }
        if (content_type != nullptr) response.content_type = content_type;
        response.body = std::move(context.body);
        return response;
    }
};
#endif

} // namespace

TransportError::TransportError(TransportErrorCode code)
    : std::runtime_error(safeTransportMessage(code)), code_(code) {}

TransportErrorCode TransportError::code() const noexcept { return code_; }

HttpClient::HttpClient(std::shared_ptr<const HttpTransport> transport,
                       HttpRequestOptions options)
    : transport_(std::move(transport)), options_(options) {
    validateOptions(options_);
}

nlohmann::json HttpClient::postJson(const std::string& url,
                                    const nlohmann::json& body,
                                    const std::string& bearer_token) const {
    std::string encoded;
    try { encoded = body.dump(); }
    catch (...) { throw std::runtime_error("HTTP request JSON encoding failed"); }
    if (encoded.size() > kMaxRequestBytes) {
        throw std::invalid_argument("HTTP request body exceeded the size limit");
    }
    return executeJson(HttpMethod::Post, url, encoded, bearer_token, false);
}

nlohmann::json HttpClient::getJson(const std::string& url) const {
    return executeJson(HttpMethod::Get, url, "", "", true);
}

nlohmann::json HttpClient::executeJson(HttpMethod method,
                                       const std::string& url,
                                       const std::string& body,
                                       const std::string& bearer_token,
                                       bool idempotent) const {
    validateUrl(url);
    validateBearerToken(bearer_token);
    if (!transport_) throw TransportError(TransportErrorCode::Unavailable);
    HttpRequest request;
    request.method = method;
    request.url = url;
    request.body = body;
    request.connect_timeout = options_.connect_timeout;
    request.request_timeout = options_.request_timeout;
    request.max_response_bytes = options_.max_response_bytes;
    request.idempotent = idempotent;
    request.retry_policy = RetryPolicy{RetryMode::Never, 1U};
    request.headers.emplace_back("Accept", "application/json");
    if (method == HttpMethod::Post) {
        request.headers.emplace_back("Content-Type", "application/json");
    }
    if (!bearer_token.empty()) {
        request.headers.emplace_back("Authorization", "Bearer " + bearer_token);
    }
    HttpResponse response;
    try { response = transport_->execute(request); }
    catch (const TransportError& error) { throw TransportError(error.code()); }
    catch (...) { throw TransportError(TransportErrorCode::Network); }
    if (response.body.size() > options_.max_response_bytes) {
        throw TransportError(TransportErrorCode::ResponseTooLarge);
    }
    if (response.status_code < 200L || response.status_code >= 300L) {
        std::ostringstream message;
        message << "HTTP status " << response.status_code;
        throw std::runtime_error(message.str());
    }
    if (!isJsonContentType(response.content_type)) {
        throw std::runtime_error("HTTP response content type is not JSON");
    }
    nlohmann::json parsed;
    try { parsed = nlohmann::json::parse(response.body); }
    catch (...) { throw std::runtime_error("HTTP response contained malformed JSON"); }
    if (!parsed.is_object()) {
        throw std::runtime_error("HTTP response JSON root must be an object");
    }
    return parsed;
}

bool HttpClient::libcurlAvailable() noexcept {
#if ELIZAOS_DGEN_KOBOLD_HAS_LIBCURL
    return true;
#else
    return false;
#endif
}

std::shared_ptr<const HttpTransport> HttpClient::createLibcurlTransport() {
#if ELIZAOS_DGEN_KOBOLD_HAS_LIBCURL
    ensureCurlInitialized();
    return std::make_shared<LibcurlTransport>();
#else
    throw TransportError(TransportErrorCode::Unavailable);
#endif
}

} // namespace elizaos
