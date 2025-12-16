#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct EnvVarEntry {
    std::string key;
    std::string value;
    std::optional<std::string> comment;
};

struct WriteOptions {
    std::optional<bool> preserveComments;
    std::optional<bool> createBackup;
    std::optional<bool> updateProcessEnv;
};

struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
};

/**
 * Unified service for managing environment files
 */
class EnvFileService {
public:
    EnvFileService(std::optional<std::string> filePath);
    std::future<void> initialize();
    std::string getFilePath();
    std::future<std::unordered_map<std::string, std::string>> read();
    std::future<std::vector<EnvVarEntry>> readWithComments();
    std::future<void> write(Record<string vars, auto string>, WriteOptions = {} options);
    std::future<void> update(const std::string& key, const std::string& value, WriteOptions = {} options);
    std::future<void> updateMany(Record<string updates, auto string>, WriteOptions = {} options);
    std::future<void> delete(const std::string& key);
    std::future<bool> exists(const std::string& key);
    std::future<std::string> backup();
    std::future<ValidationResult> validate();

// Singleton instance for global use

/**
 * Get the global EnvFileService instance
 */
std::future<EnvFileService> getEnvFileService();

/**
 * Create a new EnvFileService instance for a specific file
 */
EnvFileService createEnvFileService(const std::string& filePath);

} // namespace elizaos
