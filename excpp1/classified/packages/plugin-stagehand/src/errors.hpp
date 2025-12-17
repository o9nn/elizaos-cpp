#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class StagehandError {
public:
    StagehandError(const std::string& message, const std::string& public code, const std::string& public userMessage, boolean = true isRetryable, std::optional<std::any> details);

private:
    bool recoverable_;
};

class BrowserServiceNotAvailableError {
public:
    BrowserServiceNotAvailableError();
};

class BrowserSessionError {
public:
    BrowserSessionError(const std::string& message, std::optional<std::any> details);
};

class BrowserNavigationError {
public:
    BrowserNavigationError(const std::string& url, std::optional<Error> originalError);
};

class BrowserActionError {
public:
    BrowserActionError(const std::string& action, const std::string& target, std::optional<Error> originalError);
};

class BrowserSecurityError {
public:
    BrowserSecurityError(const std::string& message, std::optional<std::any> details);
};


} // namespace elizaos
