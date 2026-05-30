#include "curl-transport.hpp"

#include <curl/curl.h>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <thread>

namespace elizaos {
namespace eliza_api_client {

// Write callback for curl to capture response body
size_t CurlTransport::writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realSize = size * nmemb;
    auto* response = static_cast<std::string*>(userp);
    response->append(static_cast<char*>(contents), realSize);
    return realSize;
}

// Header callback for curl to capture response headers
size_t CurlTransport::headerCallback(char* buffer, size_t size, size_t nitems, void* userdata) {
    size_t totalSize = size * nitems;
    auto* headers = static_cast<std::map<std::string, std::string>*>(userdata);
    
    std::string headerLine(buffer, totalSize);
    // Remove trailing CRLF
    while (!headerLine.empty() && (headerLine.back() == '\r' || headerLine.back() == '\n')) {
        headerLine.pop_back();
    }
    
    // Skip empty lines and HTTP status line
    if (headerLine.empty() || headerLine.find("HTTP/") == 0) {
        return totalSize;
    }
    
    // Parse "Key: Value" format
    auto colonPos = headerLine.find(':');
    if (colonPos != std::string::npos) {
        std::string key = headerLine.substr(0, colonPos);
        std::string value = headerLine.substr(colonPos + 1);
        
        // Trim whitespace from key and value
        while (!key.empty() && std::isspace(static_cast<unsigned char>(key.back()))) {
            key.pop_back();
        }
        while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front()))) {
            value.erase(value.begin());
        }
        
        // Convert key to lowercase for case-insensitive lookup
        std::transform(key.begin(), key.end(), key.begin(), 
                       [](unsigned char c) { return std::tolower(c); });
        
        (*headers)[key] = value;
    }
    
    return totalSize;
}

CurlTransport::CurlTransport() {
    initializeCurl();
}

CurlTransport::CurlTransport(const CurlTransportConfig& config)
    : config_(config) {
    initializeCurl();
}

CurlTransport::~CurlTransport() {
    cleanupCurl();
}

CurlTransport::CurlTransport(CurlTransport&& other) noexcept
    : config_(std::move(other.config_))
    , curlHandle_(other.curlHandle_)
    , initialized_(other.initialized_)
    , totalRequests_(other.totalRequests_)
    , successfulRequests_(other.successfulRequests_)
    , failedRequests_(other.failedRequests_)
    , retriedRequests_(other.retriedRequests_)
    , totalRequestTime_(other.totalRequestTime_)
    , lastRequestTime_(other.lastRequestTime_) {
    other.curlHandle_ = nullptr;
    other.initialized_ = false;
}

CurlTransport& CurlTransport::operator=(CurlTransport&& other) noexcept {
    if (this != &other) {
        cleanupCurl();
        config_ = std::move(other.config_);
        curlHandle_ = other.curlHandle_;
        initialized_ = other.initialized_;
        totalRequests_ = other.totalRequests_;
        successfulRequests_ = other.successfulRequests_;
        failedRequests_ = other.failedRequests_;
        retriedRequests_ = other.retriedRequests_;
        totalRequestTime_ = other.totalRequestTime_;
        lastRequestTime_ = other.lastRequestTime_;
        other.curlHandle_ = nullptr;
        other.initialized_ = false;
    }
    return *this;
}

bool CurlTransport::initializeCurl() {
    if (initialized_) {
        return true;
    }

    CURL* handle = curl_easy_init();
    if (!handle) {
        return false;
    }

    curlHandle_ = handle;
    initialized_ = true;
    return true;
}

void CurlTransport::cleanupCurl() {
    if (curlHandle_) {
        curl_easy_cleanup(static_cast<CURL*>(curlHandle_));
        curlHandle_ = nullptr;
    }
    initialized_ = false;
}

bool CurlTransport::isReady() const {
    return initialized_ && curlHandle_ != nullptr;
}

std::string CurlTransport::buildQueryString(const nlohmann::json& query) const {
    if (!query.is_object() || query.empty()) {
        return "";
    }

    std::ostringstream oss;
    bool first = true;
    
    for (auto it = query.begin(); it != query.end(); ++it) {
        if (!first) {
            oss << "&";
        }
        first = false;
        
        // URL-encode the key
        char* encodedKey = curl_easy_escape(static_cast<CURL*>(curlHandle_), 
                                            it.key().c_str(), 
                                            static_cast<int>(it.key().length()));
        if (encodedKey) {
            oss << encodedKey;
            curl_free(encodedKey);
        } else {
            oss << it.key();
        }
        
        oss << "=";
        
        // Convert value to string and URL-encode
        std::string valueStr;
        if (it.value().is_string()) {
            valueStr = it.value().get<std::string>();
        } else {
            valueStr = it.value().dump();
        }
        
        char* encodedValue = curl_easy_escape(static_cast<CURL*>(curlHandle_),
                                              valueStr.c_str(),
                                              static_cast<int>(valueStr.length()));
        if (encodedValue) {
            oss << encodedValue;
            curl_free(encodedValue);
        } else {
            oss << valueStr;
        }
    }

    return oss.str();
}

bool CurlTransport::shouldRetry(long httpCode, int attemptNumber) const {
    if (attemptNumber >= config_.maxRetries) {
        return false;
    }
    
    // Retry on server errors and specific client errors
    if (httpCode >= 500 && httpCode < 600) {
        return true;  // Server errors
    }
    if (httpCode == 429) {
        return true;  // Rate limited
    }
    if (httpCode == 408) {
        return true;  // Request timeout
    }
    
    return false;
}

ApiResult CurlTransport::performRequest(const HttpRequest& request) {
    auto startTime = std::chrono::steady_clock::now();
    
    if (!isReady()) {
        ++failedRequests_;
        return ApiResult::failure("transport_not_initialized", 
                                  "CurlTransport is not initialized.");
    }

    CURL* curl = static_cast<CURL*>(curlHandle_);
    
    // Reset curl handle for new request
    curl_easy_reset(curl);

    // Build URL with query string
    std::string fullUrl = request.url;
    std::string queryString = buildQueryString(request.query);
    if (!queryString.empty()) {
        fullUrl += (fullUrl.find('?') == std::string::npos ? "?" : "&") + queryString;
    }

    // Set URL
    curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());

    // Set method
    std::string methodUpper = request.method;
    std::transform(methodUpper.begin(), methodUpper.end(), methodUpper.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    
    if (methodUpper == "GET") {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    } else if (methodUpper == "POST") {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
    } else if (methodUpper == "PUT") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    } else if (methodUpper == "DELETE") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    } else if (methodUpper == "PATCH") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    } else if (methodUpper == "HEAD") {
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    } else {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, methodUpper.c_str());
    }

    // Set request body
    std::string bodyStr;
    if (!request.body.is_null() && !request.body.empty()) {
        bodyStr = request.body.dump();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(bodyStr.length()));
    }

    // Set headers
    struct curl_slist* headers = nullptr;
    
    // Add default headers
    for (const auto& [key, value] : config_.defaultHeaders) {
        std::string header = key + ": " + value;
        headers = curl_slist_append(headers, header.c_str());
    }
    
    // Add request-specific headers
    bool hasContentType = false;
    for (const auto& [key, value] : request.headers) {
        std::string keyLower = key;
        std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        if (keyLower == "content-type") {
            hasContentType = true;
        }
        std::string header = key + ": " + value;
        headers = curl_slist_append(headers, header.c_str());
    }
    
    // Add default Content-Type for requests with body
    if (!hasContentType && !bodyStr.empty()) {
        headers = curl_slist_append(headers, "Content-Type: application/json");
    }
    
    // Add Accept header if not present
    headers = curl_slist_append(headers, "Accept: application/json");
    
    if (headers) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    // Set User-Agent
    curl_easy_setopt(curl, CURLOPT_USERAGENT, config_.userAgent.c_str());

    // Set timeouts
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, static_cast<long>(config_.timeoutMs));
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, static_cast<long>(config_.connectTimeoutMs));

    // Set SSL options
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, config_.verifySSL ? 1L : 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, config_.verifySSL ? 2L : 0L);

    // Set redirect options
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, config_.followRedirects ? 1L : 0L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, static_cast<long>(config_.maxRedirects));

    // Set up response capture
    std::string responseBody;
    std::map<std::string, std::string> responseHeaders;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &responseHeaders);

    // Perform request with retry logic
    CURLcode res = CURLE_OK;
    long httpCode = 0;
    int attempts = 0;
    
    do {
        if (attempts > 0) {
            ++retriedRequests_;
            // Wait before retry
            std::this_thread::sleep_for(std::chrono::milliseconds(config_.retryDelayMs * attempts));
            responseBody.clear();
            responseHeaders.clear();
        }
        
        res = curl_easy_perform(curl);
        
        if (res == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        }
        
        ++attempts;
    } while (res == CURLE_OK && shouldRetry(httpCode, attempts));

    // Clean up headers
    if (headers) {
        curl_slist_free_all(headers);
    }

    // Calculate request time
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    totalRequestTime_ += duration;
    lastRequestTime_ = std::chrono::system_clock::now();
    ++totalRequests_;

    // Handle curl errors
    if (res != CURLE_OK) {
        ++failedRequests_;
        std::string errorMsg = curl_easy_strerror(res);
        return ApiResult::failure("curl_error", errorMsg);
    }

    // Parse and return response
    return parseResponse(responseBody, responseHeaders, httpCode);
}

ApiResult CurlTransport::parseResponse(const std::string& responseBody,
                                       const std::map<std::string, std::string>& responseHeaders,
                                       long httpCode) const {
    HttpResponse response;
    response.statusCode = static_cast<int>(httpCode);
    response.headers = responseHeaders;
    response.body = responseBody;

    // Try to parse JSON body
    auto contentTypeIt = responseHeaders.find("content-type");
    bool isJson = contentTypeIt != responseHeaders.end() && 
                  contentTypeIt->second.find("application/json") != std::string::npos;
    
    if (isJson && !responseBody.empty()) {
        try {
            response.jsonBody = nlohmann::json::parse(responseBody);
        } catch (const nlohmann::json::parse_error&) {
            // Not valid JSON, leave jsonBody as empty object
        }
    }

    // Determine success based on HTTP status code
    if (httpCode >= 200 && httpCode < 300) {
        ++successfulRequests_;
        return ApiResult::success(std::move(response));
    } else {
        ++failedRequests_;
        std::string errorCode = "http_" + std::to_string(httpCode);
        std::string errorMessage = "HTTP request failed with status " + std::to_string(httpCode);
        
        // Try to extract error message from response body
        if (!response.jsonBody.is_null() && response.jsonBody.contains("error")) {
            if (response.jsonBody["error"].is_string()) {
                errorMessage = response.jsonBody["error"].get<std::string>();
            } else if (response.jsonBody["error"].is_object() && 
                       response.jsonBody["error"].contains("message")) {
                errorMessage = response.jsonBody["error"]["message"].get<std::string>();
            }
        } else if (!response.jsonBody.is_null() && response.jsonBody.contains("message")) {
            errorMessage = response.jsonBody["message"].get<std::string>();
        }
        
        return ApiResult::failure(errorCode, errorMessage, std::move(response));
    }
}

ApiResult CurlTransport::send(const HttpRequest& request) {
    return performRequest(request);
}

nlohmann::json CurlTransport::getDiagnostics() const {
    nlohmann::json diagnostics;
    diagnostics["name"] = getName();
    diagnostics["initialized"] = initialized_;
    diagnostics["curlHandleValid"] = curlHandle_ != nullptr;
    
    if (curlHandle_) {
        curl_version_info_data* info = curl_version_info(CURLVERSION_NOW);
        if (info) {
            diagnostics["curlVersion"] = info->version;
            diagnostics["sslVersion"] = info->ssl_version ? info->ssl_version : "none";
            diagnostics["protocols"] = nlohmann::json::array();
            if (info->protocols) {
                for (int i = 0; info->protocols[i]; ++i) {
                    diagnostics["protocols"].push_back(info->protocols[i]);
                }
            }
        }
    }
    
    diagnostics["config"] = nlohmann::json{
        {"timeoutMs", config_.timeoutMs},
        {"connectTimeoutMs", config_.connectTimeoutMs},
        {"maxRetries", config_.maxRetries},
        {"retryDelayMs", config_.retryDelayMs},
        {"verifySSL", config_.verifySSL},
        {"followRedirects", config_.followRedirects},
        {"maxRedirects", config_.maxRedirects},
        {"userAgent", config_.userAgent}
    };
    
    return diagnostics;
}

nlohmann::json CurlTransport::getStats() const {
    nlohmann::json stats;
    stats["totalRequests"] = totalRequests_;
    stats["successfulRequests"] = successfulRequests_;
    stats["failedRequests"] = failedRequests_;
    stats["retriedRequests"] = retriedRequests_;
    stats["totalRequestTimeMs"] = totalRequestTime_.count();
    
    if (totalRequests_ > 0) {
        stats["averageRequestTimeMs"] = totalRequestTime_.count() / totalRequests_;
        stats["successRate"] = static_cast<double>(successfulRequests_) / totalRequests_;
    } else {
        stats["averageRequestTimeMs"] = 0;
        stats["successRate"] = 0.0;
    }
    
    if (lastRequestTime_ != std::chrono::system_clock::time_point{}) {
        auto now = std::chrono::system_clock::now();
        auto sinceLastRequest = std::chrono::duration_cast<std::chrono::seconds>(
            now - lastRequestTime_).count();
        stats["secondsSinceLastRequest"] = sinceLastRequest;
    }
    
    return stats;
}

void CurlTransport::resetStats() {
    totalRequests_ = 0;
    successfulRequests_ = 0;
    failedRequests_ = 0;
    retriedRequests_ = 0;
    totalRequestTime_ = std::chrono::milliseconds{0};
    lastRequestTime_ = std::chrono::system_clock::time_point{};
}

std::shared_ptr<IHttpTransport> createCurlTransport() {
    return std::make_shared<CurlTransport>();
}

std::shared_ptr<IHttpTransport> createCurlTransport(const CurlTransportConfig& config) {
    return std::make_shared<CurlTransport>(config);
}

} // namespace eliza_api_client
} // namespace elizaos
