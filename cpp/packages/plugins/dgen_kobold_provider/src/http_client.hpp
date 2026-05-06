#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace elizaos {

/**
 * Simple HTTP client for making API requests to LLM providers.
 * Uses libcurl when available, otherwise provides a stub implementation.
 */
class HttpClient {
public:
    /**
     * Make a POST request with JSON body.
     * @param url The endpoint URL
     * @param body The JSON request body
     * @param api_key Optional Bearer token for authentication
     * @return Parsed JSON response
     * @throws std::runtime_error on HTTP or parsing errors
     */
    static json post(const std::string& url, const json& body, const std::string& api_key = "");
};

} // namespace elizaos
