#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "errors.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Security configuration and utilities for the browser plugin
 */

/**
 * Security configuration
 */
struct SecurityConfig {
    std::optional<std::vector<std::string>> allowedDomains;
    std::optional<std::vector<std::string>> blockedDomains;
    std::optional<double> maxUrlLength;
    std::optional<bool> allowLocalhost;
    std::optional<bool> allowFileProtocol;
    std::optional<bool> sanitizeInputs;
};

/**
 * Default security configuration
 */
    // Common malicious domains
    // Add more as needed

/**
 * URL validation and sanitization
 */
class UrlValidator {
public:
    UrlValidator(std::optional<SecurityConfig> config = {});
    void updateConfig(const std::optional<SecurityConfig>& config);

/**
 * Input sanitization utilities
 */
class InputSanitizer {
public:
    static std::string sanitizeText(const std::string& input);
    static std::string sanitizeSelector(const std::string& selector);
    static std::string sanitizeFilePath(const std::string& path);
};

/**
 * Security middleware for actions
 */
void validateSecureAction(const std::string& url, UrlValidator validator);

/**
 * Create a default URL validator instance
 */

/**
 * Rate limiting configuration
 */
struct RateLimitConfig {
    double maxActionsPerMinute;
    double maxSessionsPerHour;
};

/**
 * Simple rate limiter
 */
class RateLimiter {
public:
    RateLimiter();
    bool checkActionLimit(const std::string& userId);
    bool checkSessionLimit(const std::string& userId);
};


} // namespace elizaos
