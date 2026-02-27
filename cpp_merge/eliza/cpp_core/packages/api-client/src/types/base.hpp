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
    std: code;
    std: message;
    std::optional<std:> details;
};

struct RequestConfig {
    std::optional<std::unordered_map<std:, std:>> headers;
    std::optional<std::unordered_map<std:, std:>> params;
    std::optional<double> timeout;
};

struct PaginationParams {
    std::optional<double> page;
    std::optional<double> limit;
    std::optional<double> offset;
};

struct ApiClientConfig {
    std: baseUrl;
    std::optional<std:> apiKey;
    std::optional<double> timeout;
    std::optional<std::unordered_map<std:, std:>> headers;
};


} // namespace elizaos
