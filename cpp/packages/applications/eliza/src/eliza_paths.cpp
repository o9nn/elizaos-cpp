// eliza_paths.cpp - Stub implementation
#include "elizaos/core.hpp"
#include <string>
#include <filesystem>

namespace elizaos {
namespace eliza {

std::string getElizaDataPath() {
    return std::filesystem::current_path().string() + "/.eliza";
}

std::string getElizaConfigPath() {
    return getElizaDataPath() + "/config";
}

std::string getElizaLogsPath() {
    return getElizaDataPath() + "/logs";
}

} // namespace eliza
} // namespace elizaos
