#pragma once

#include <any>
#include <cmath>
#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace elizaos {

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
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};

struct SessionUploadOptions {
    std::optional<std::string> endpoint;
    std::optional<int> retries;
    std::optional<int> timeout;
    std::optional<bool> failOnError;
};

struct UploadResult {
    bool success;
    std::optional<std::string> error;
    int statusCode;
};

/**
 * Sessions client for uploading session logs with defensive error handling
 */
class SessionsClient {
private:
    std::string defaultEndpoint_;
    SessionUploadOptions defaultOptions_;

public:
    /**
     * Constructor
     * @param endpoint - The endpoint URL for session logs
     * @param options - Upload options
     */
    explicit SessionsClient(const std::string& endpoint = "", 
                           const SessionUploadOptions& options = {})
        : defaultEndpoint_(endpoint)
        , defaultOptions_(options) {
        // Set default values if not provided
        if (!defaultOptions_.retries.has_value()) {
            defaultOptions_.retries = 3;
        }
        if (!defaultOptions_.timeout.has_value()) {
            defaultOptions_.timeout = 10000;
        }
        if (!defaultOptions_.failOnError.has_value()) {
            defaultOptions_.failOnError = false;
        }
    }

    /**
     * Upload session logs with defensive error handling
     * @param logData - The session log data to upload
     * @param options - Upload options (std::optional)
     * @return true if successful, false if failed but gracefully handled
     */
    bool uploadLogs(const SessionLogData& logData, 
                    const SessionUploadOptions& options = {}) {
        auto effectiveOptions = mergeOptions(options);
        int maxRetries = effectiveOptions.retries.value_or(3);
        
        for (int attempt = 0; attempt < maxRetries; ++attempt) {
            try {
                auto result = performUpload(logData, effectiveOptions);
                
                if (result.success) {
                    return true;
                }
                
                // Handle specific HTTP status codes
                if (result.statusCode >= 400 && result.statusCode < 500) {
                    // Client errors (400-499) - don't retry
                    if (!effectiveOptions.failOnError.value_or(false)) {
                        return false;
                    }
                    break;
                }
                
                // Server errors (500+) - retry with exponential backoff
                if (attempt < maxRetries - 1) {
                    int delayMs = static_cast<int>(std::pow(2, attempt) * 1000);
                    std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
                }
            } catch (const std::exception& e) {
                // Handle network errors, timeouts, etc.
                if (attempt < maxRetries - 1) {
                    int delayMs = static_cast<int>(std::pow(2, attempt) * 1000);
                    std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
                }
            }
        }
        
        // All retries exhausted
        return !effectiveOptions.failOnError.value_or(false);
    }

    /**
     * Health check for the session logging endpoint
     * @return true if endpoint is available
     */
    bool healthCheck() {
        // Placeholder for health check implementation
        return !defaultEndpoint_.empty();
    }

    /**
     * Get the configured endpoint
     */
    std::string getEndpoint() const {
        return defaultEndpoint_;
    }

private:
    /**
     * Merge provided options with defaults
     */
    SessionUploadOptions mergeOptions(const SessionUploadOptions& options) const {
        SessionUploadOptions merged;
        merged.endpoint = options.endpoint.has_value() ? options.endpoint : 
                          (defaultOptions_.endpoint.has_value() ? defaultOptions_.endpoint : 
                           std::optional<std::string>(defaultEndpoint_));
        merged.retries = options.retries.has_value() ? options.retries : defaultOptions_.retries;
        merged.timeout = options.timeout.has_value() ? options.timeout : defaultOptions_.timeout;
        merged.failOnError = options.failOnError.has_value() ? options.failOnError : defaultOptions_.failOnError;
        return merged;
    }

    /**
     * Perform the actual HTTP upload
     */
    UploadResult performUpload(const SessionLogData& logData, 
                               const SessionUploadOptions& options) {
        // Placeholder for actual HTTP implementation
        // In a real implementation, this would use libcurl or similar
        UploadResult result;
        result.success = true;
        result.statusCode = 200;
        return result;
    }
};

} // namespace elizaos
