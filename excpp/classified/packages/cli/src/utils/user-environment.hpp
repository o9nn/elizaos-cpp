#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "auto-install-bun.hpp"
#include "bun-exec.hpp"
#include "elizaos/core.hpp"
#include "emoji-handler.hpp"
#include "resolve-utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Types
struct OSInfo {
    std::string platform;
    std::string release;
    std::string arch;
    std::string type;
    std::string version;
    std::string homedir;
};

struct CLIInfo {
    std::string version;
    std::string name;
    std::string path;
};

struct PackageManagerInfo {
    std::string name;
    std::string version;
    bool global;
    bool isNpx;
    bool isBunx;
};

struct PathInfo {
    std::string elizaDir;
    std::string envFilePath;
    std::string configPath;
    std::string pluginsDir;
    std::string monorepoRoot;
    std::string packageJsonPath;
};

struct EnvInfo {
    std::optional<std::string> GITHUB_USERNAME;
    std::optional<std::string> GITHUB_TOKEN;
};

struct UserEnvironmentInfo {
    OSInfo os;
    CLIInfo cli;
    PackageManagerInfo packageManager;
    std::string timestamp;
    PathInfo paths;
    EnvInfo env;
};

/**
 * Provides information about the user's environment including OS, CLI, and package manager details.
 * Uses singleton pattern to cache results.
 */
class UserEnvironment {
public:
    UserEnvironment();
    std::future<OSInfo> getOSInfo();
    std::future<CLIInfo> getCLIInfo();
    std::future<PackageManagerInfo> getPackageManagerInfo();
    std::string findMonorepoRoot(const std::string& startDir);
    std::future<PathInfo> getPathInfo();
    std::future<EnvInfo> getEnvInfo();
    std::future<UserEnvironmentInfo> getInfo(std::optional<std::string> directory);
    void clearCache();
    std::future<std::string> getPackageVersion(const std::string& packageName);
    std::future<std::vector<std::string>> getLocalPackages();
};


} // namespace elizaos
