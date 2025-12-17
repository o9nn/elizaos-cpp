#pragma once
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
 * Ensures a directory exists, creating it and all parent directories if needed
 */
std::future<void> ensureDir(const std::string& dirPath);

/**
 * Writes data to a file, ensuring the parent directory exists
 */
std::future<void> writeToFile(const std::string& filePath, const std::string& data);

/**
 * Generates a file path for repository data
 */
void getRepoFilePath(const std::string& outputDir, const std::string& repoId, const std::string& dataType, const std::string& intervalType, const std::string& fileName);

} // namespace elizaos
