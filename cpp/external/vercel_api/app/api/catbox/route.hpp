#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * HTTP Request representation
 */
struct HttpRequest {
    std::string method;
    std::string url;
    std::unordered_map<std::string, std::string> headers;
    std::optional<std::string> body;
    std::unordered_map<std::string, std::string> query;
};

/**
 * HTTP Response representation
 */
struct HttpResponse {
    int statusCode = 200;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

/**
 * Request error for better type safety
 */
struct RequestError {
    std::string message;
    std::optional<std::string> code;
    int status = 500;
};

/**
 * Helper function to extract file ID from catbox URL
 * @param url - The URL to extract from
 * @return The extracted file ID or empty string
 */
std::string extractFileIdFromUrl(const std::string& url);

/**
 * Handle POST requests
 * @param request - The HTTP request
 * @return Future with HTTP response
 */
std::future<HttpResponse> POST(const HttpRequest& request);

/**
 * Handle GET requests
 * @param request - The HTTP request
 * @return Future with HTTP response
 */
std::future<HttpResponse> GET(const HttpRequest& request);

/**
 * Helper function to get content type from file extension
 * @param filename - The filename to check
 * @return MIME type string
 */
std::string getContentTypeFromExtension(const std::string& filename);

/**
 * Handle CORS preflight requests
 * @return Future with HTTP response
 */
std::future<HttpResponse> OPTIONS();

} // namespace elizaos
