#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ApiErrorResponse {
    { error;
    std::string code;
    std::string message;
    std::optional<std::string> details;
};

struct RequestConfig {
    std::optional<std::unordered_map<std::string, std::string>> headers;
    std::optional<std::unordered_map<std::string, std::any>> params;
    std::optional<double> timeout;
};

struct PaginationParams {
    std::optional<double> page;
    std::optional<double> limit;
    std::optional<double> offset;
};

struct ApiClientConfig {
    std::string baseUrl;
    std::optional<std::string> apiKey;
    std::optional<double> timeout;
    std::optional<std::unordered_map<std::string, std::string>> headers;
};


} // namespace elizaos
