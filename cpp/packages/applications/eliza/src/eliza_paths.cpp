#include "elizaos/core.hpp"

#include <cstdlib>
#include <filesystem>
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

std::filesystem::path resolveBasePath() {
    if (auto explicit_path = pathFromEnvironment("ELIZAOS_DATA_DIR"); !explicit_path.empty()) {
        return explicit_path;
    }

    if (auto xdg_state_home = pathFromEnvironment("XDG_STATE_HOME"); !xdg_state_home.empty()) {
        return xdg_state_home / "elizaos";
    }

    return std::filesystem::current_path() / ".eliza";
}

std::string ensureDirectory(std::filesystem::path path) {
    std::error_code ec;
    std::filesystem::create_directories(path, ec);
    return path.lexically_normal().string();
}

} // namespace

std::string getElizaDataPath() {
    return ensureDirectory(resolveBasePath());
}

std::string getElizaConfigPath() {
    return ensureDirectory(resolveBasePath() / "config");
}

std::string getElizaLogsPath() {
    return ensureDirectory(resolveBasePath() / "logs");
}

} // namespace eliza
} // namespace elizaos
