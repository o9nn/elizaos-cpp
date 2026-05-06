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

/**
 * Update command types and interfaces
 */

/**
 * Command options for update operations
 */
struct UpdateOptions {
    std::optional<bool> dryRun;
    std::optional<bool> skipBuild;
    std::optional<bool> skipBunMigration;
};

/**
 * Global CLI update options
 */
struct GlobalUpdateOptions {
    std::optional<std::string> version;
    std::optional<bool> skipBunMigration;
};

/**
 * Version comparison result
 */
struct VersionCheckResult {
    bool needsUpdate;
    std::optional<std::string> error;
};

/**
 * Package update information
 */
struct PackageUpdate {
    std::string current;
    std::string latest;
};

/**
 * Update check result
 */
struct UpdateCheckResult {
    bool hasUpdates;
    std::unordered_map<std::string, PackageUpdate> updates;
};

/**
 * CLI environment information
 */
struct CliEnvironment {
    bool isGlobal;
    bool isNpx;
    bool isBunx;
    bool isNpmInstalled;
    std::string packageManager;
};

/**
 * Update context information
 */
struct UpdateContext {
    std::string cwd;
    bool isPlugin;
    import('@/src/utils/directory-detection').DirectoryInfo directoryInfo;
    CliEnvironment environment;
};


} // namespace elizaos
