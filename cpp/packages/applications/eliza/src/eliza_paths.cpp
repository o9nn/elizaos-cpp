#include "elizaos/core.hpp"

#include <cstdlib>
#include <filesystem>
#include <mutex>
#include <optional>
#include <string>

namespace elizaos {
namespace eliza {
namespace {

std::filesystem::path pathFromEnvironment(const char* variable) {
    if (const char* value = std::getenv(variable)) {
        if (*value != '\0') {
            return std::filesystem::path(value);
        }
    }
    return {};
}

// Resolve the base data directory honoring (in priority order):
//   1. ELIZAOS_ROOT_DIR   - explicit root for all eliza data
//   2. ELIZAOS_DATA_DIR   - legacy explicit data directory
//   3. XDG_STATE_HOME/elizaos - XDG base directory spec
//   4. <cwd>/.eliza       - project-local fallback
std::filesystem::path resolveBasePath() {
    if (auto root_dir = pathFromEnvironment("ELIZAOS_ROOT_DIR"); !root_dir.empty()) {
        return root_dir;
    }

    if (auto explicit_path = pathFromEnvironment("ELIZAOS_DATA_DIR"); !explicit_path.empty()) {
        return explicit_path;
    }

    if (auto xdg_state_home = pathFromEnvironment("XDG_STATE_HOME"); !xdg_state_home.empty()) {
        return xdg_state_home / "elizaos";
    }

    return std::filesystem::current_path() / ".eliza";
}

// Cached base path so repeated getter calls return stable results within a
// process even if the environment or working directory changes mid-run. The
// cache is invalidated explicitly via resetPathCache().
std::mutex& cacheMutex() {
    static std::mutex m;
    return m;
}

std::optional<std::filesystem::path>& cachedBasePath() {
    static std::optional<std::filesystem::path> base;
    return base;
}

std::filesystem::path basePath() {
    std::lock_guard<std::mutex> lock(cacheMutex());
    auto& cached = cachedBasePath();
    if (!cached.has_value()) {
        cached = resolveBasePath();
    }
    return *cached;
}

std::string ensureDirectory(std::filesystem::path path) {
    std::error_code ec;
    std::filesystem::create_directories(path, ec);
    return path.lexically_normal().string();
}

} // namespace

void resetPathCache() {
    std::lock_guard<std::mutex> lock(cacheMutex());
    cachedBasePath().reset();
}

std::string getElizaDataPath() {
    return ensureDirectory(basePath());
}

std::string getElizaConfigPath() {
    return ensureDirectory(basePath() / "config");
}

std::string getElizaLogsPath() {
    return ensureDirectory(basePath() / "logs");
}

std::string getElizaCharactersPath() {
    return ensureDirectory(basePath() / "characters");
}

std::string getElizaKnowledgePath() {
    return ensureDirectory(basePath() / "knowledge");
}

std::string getElizaPluginsPath() {
    return ensureDirectory(basePath() / "plugins");
}

std::string getElizaStatePath() {
    return ensureDirectory(basePath() / "state");
}

} // namespace eliza
} // namespace elizaos
