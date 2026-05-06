#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "auth/index.hpp"
#include "elizaos/core.hpp"
#include "providers/elizaos-provider.js.hpp"
#include "tests.hpp"
#include "tests/real-integration.test.hpp"
#include "tests/storage-integration.test.hpp"
#include "tests/validation-summary.test.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Removed multi-provider imports - now using ElizaOS API service directly

/**
 */
std::future<void> importAWSSDK();

/**
 * Configuration schema for ElizaOS Services plugin
 */

/**
 * Helper functions for API interactions
 */
std::string getSetting(IAgentRuntime runtime, const std::string& key, std::optional<std::string> defaultValue);

void getStorageConfig(IAgentRuntime runtime);

/**
 * Create S3 client for storage operations
 */

/**
 * Storage service for file uploads and management
 */
class ElizaOSStorageService {
public:
    ElizaOSStorageService(IAgentRuntime runtime);
    std::future<void> ensureInitialized();
    std::future<std::string> uploadFile(const std::string& key, const std::vector<uint8_t>& data, std::optional<std::string> contentType);
    std::future<std::vector<uint8_t>> downloadFile(const std::string& key);
    std::future<void> deleteFile(const std::string& key);
    std::future<bool> fileExists(const std::string& key);
    std::future<std::vector<std::string>> listFiles(std::optional<std::string> prefix, double maxKeys = 1000);
    Promise< getFileMetadata(const std::string& key);

private:
    std::string bucket_;
    IAgentRuntime runtime_;
};

/**
 * ElizaOS Services main service class
 */
class ElizaOSService {
public:
    ElizaOSService(IAgentRuntime runtime);
    static std::future<ElizaOSService> start(IAgentRuntime runtime);
    std::future<void> stop();
    ElizaOSStorageService getStorage();

private:
    ElizaOSStorageService storage_;
};

/**
 * ElizaOS Services Plugin - hosted AI inference and storage
 */

      // Set environment variables

      // Initialize authentication system

      // Perform comprehensive authentication validation

        // Check production readiness

      // Register CLI commands if in CLI environment

      // Legacy provider validation (for backward compatibility)

      // Validate storage configuration if provided

      // Final health check

        // Extract embedding from OpenAI-compatible response

        // Check if custom prompt - return raw content

        // Parse for title/description format

        // Try to parse JSON from content

        // Try to parse JSON from content


} // namespace elizaos
