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
    std::optional<std:> version;
    std::optional<bool> skipBunMigration;
};

/**
 * Version comparison result
 */
struct VersionCheckResult {
    bool needsUpdate;
    std::optional<std:> error;
};

/**
 * Package update information
 */
struct PackageUpdate {
    std: current;
    std: latest;
};

/**
 * Update check result
 */
struct UpdateCheckResult {
    bool hasUpdates;
};

/**
 * CLI environment information
 */
struct CliEnvironment {
    bool isGlobal;
    bool isNpx;
    bool isBunx;
    bool isNpmInstalled;
    std: packageManager;
};

/**
 * Update context information
 */
struct UpdateContext {
    std: cwd;
    bool isPlugin;
    CliEnvironment environment;
};


} // namespace elizaos
