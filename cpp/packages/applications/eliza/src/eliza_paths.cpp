// eliza_paths.cpp - Environment-aware path resolution for ElizaOS
//
// Path resolution precedence:
//   1. ELIZAOS_ROOT_DIR  -> explicit root override (highest priority)
//   2. ELIZAOS_DATA_DIR  -> explicit data directory
//   3. Current working directory / ".eliza" (default)
//
// All paths are created idempotently and returned normalized.

#include "elizaos/core.hpp"
#include <string>
#include <filesystem>
#include <cstdlib>
#include <mutex>

namespace elizaos {
namespace eliza {

namespace {

// Thread-safe lazy initialization of the root path
std::mutex pathMutex;
std::string cachedRootPath;
bool rootInitialized = false;

std::string resolveRootPath() {
    // Priority 1: ELIZAOS_ROOT_DIR
    if (const char* rootDir = std::getenv("ELIZAOS_ROOT_DIR")) {
        std::filesystem::path p(rootDir);
        return p.lexically_normal().string();
    }
    // Priority 2: ELIZAOS_DATA_DIR
    if (const char* dataDir = std::getenv("ELIZAOS_DATA_DIR")) {
        std::filesystem::path p(dataDir);
        return p.lexically_normal().string();
    }
    // Priority 3: CWD/.eliza
    std::filesystem::path p = std::filesystem::current_path() / ".eliza";
    return p.lexically_normal().string();
}

std::string ensureDirectory(const std::string& path) {
    std::filesystem::path p(path);
    if (!std::filesystem::exists(p)) {
        std::filesystem::create_directories(p);
    }
    return p.lexically_normal().string();
}

} // anonymous namespace

std::string getElizaDataPath() {
    std::lock_guard<std::mutex> lock(pathMutex);
    if (!rootInitialized) {
        cachedRootPath = resolveRootPath();
        rootInitialized = true;
    }
    return ensureDirectory(cachedRootPath);
}

std::string getElizaConfigPath() {
    std::filesystem::path p = std::filesystem::path(getElizaDataPath()) / "config";
    return ensureDirectory(p.lexically_normal().string());
}

std::string getElizaLogsPath() {
    std::filesystem::path p = std::filesystem::path(getElizaDataPath()) / "logs";
    return ensureDirectory(p.lexically_normal().string());
}

std::string getElizaCharactersPath() {
    std::filesystem::path p = std::filesystem::path(getElizaDataPath()) / "characters";
    return ensureDirectory(p.lexically_normal().string());
}

std::string getElizaKnowledgePath() {
    std::filesystem::path p = std::filesystem::path(getElizaDataPath()) / "knowledge";
    return ensureDirectory(p.lexically_normal().string());
}

std::string getElizaPluginsPath() {
    std::filesystem::path p = std::filesystem::path(getElizaDataPath()) / "plugins";
    return ensureDirectory(p.lexically_normal().string());
}

std::string getElizaStatePath() {
    std::filesystem::path p = std::filesystem::path(getElizaDataPath()) / "state";
    return ensureDirectory(p.lexically_normal().string());
}

void resetPathCache() {
    std::lock_guard<std::mutex> lock(pathMutex);
    rootInitialized = false;
    cachedRootPath.clear();
}

} // namespace eliza
} // namespace elizaos
