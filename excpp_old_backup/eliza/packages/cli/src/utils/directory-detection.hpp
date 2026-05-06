#include "user-environment.hpp"
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



struct DirectoryInfo {
    bool hasPackageJson;
    bool hasElizaOSDependencies;
    std::optional<std::string> packageName;
    double elizaPackageCount;
    std::optional<std::string> monorepoRoot;
};

struct PackageJson {
    std::optional<std::string> name;
    std::optional<std::vector<std::string>> keywords;
    std::optional<std::string> main;
    std::optional<std::unordered_map<std::string, std::string>> dependencies;
    std::optional<std::unordered_map<std::string, std::string>> devDependencies;
    std::optional<string[] | { packages?: string[] }> workspaces;

/**
 * Detects the type of directory and provides comprehensive information about it
 * @param dir The directory path to analyze
 * @returns DirectoryInfo object with detection results
 */
DirectoryInfo detectDirectoryType(const std::string& dir);

/**
 * Checks if a package.json indicates an ElizaOS plugin
 */
bool isElizaOSPlugin(PackageJson packageJson);

/**
 * Checks if a package.json and directory structure indicates an ElizaOS project
 */
bool isElizaOSProject(PackageJson packageJson, const std::string& dir, std::optional<std::string> monorepoRoot);

/**
 * Checks if the directory is suitable for ElizaOS package updates
 */
bool isValidForUpdates(DirectoryInfo info);

} // namespace elizaos
