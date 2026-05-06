#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


struct ApiErrorResponse {
    std::string code;
    std::string message;
    std::optional<std::string> details;
};

struct RequestConfig {
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
};


} // namespace elizaos
