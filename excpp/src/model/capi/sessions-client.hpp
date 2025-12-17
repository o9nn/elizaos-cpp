#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Defensive Sessions Client with 404 Error Handling
 * 
 * This module provides robust session log upload functionality that gracefully
 * handles endpoint unavailability (404 errors) without failing the entire workflow.
 * 
 * Fixes GitHub Issue #27: https://github.com/ZoneCog/elizaos-cpp/issues/27
 */

struct SessionLogData {
    std::string sessionId;
    std::vector<std::any> logs;
    double timestamp;
};

struct SessionUploadOptions {
    std::optional<std::string> endpoint;
    std::optional<double> retries;
    std::optional<double> timeout;
    std::optional<bool> failOnError;
};

class SessionsClient {
public:
    SessionsClient(std::optional<std::string> endpoint, SessionUploadOptions options = {});
    std::future<bool> uploadSessionLog(SessionLogData logData, SessionUploadOptions options = {});
    std::future<Response> performUpload(const std::string& endpoint, SessionLogData logData, std::optional<double> timeoutMs);
    std::future<void> delay(double ms);
    std::future<bool> healthCheck(std::optional<std::string> endpoint);

// Export for use in workflows and other modules

// Default for direct usage

// CLI usage when run as a script
  
  // Example usage
  
} // namespace elizaos
