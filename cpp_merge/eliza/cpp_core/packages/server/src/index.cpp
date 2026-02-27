#include "index.hpp"
#include <string>
#include <filesystem>
#include <cstdlib>
#include <optional>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string expandTildePath(const std::string& filepath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!filepath) {
        return filepath;
    }

    if (filepath.substr(0, '~')) {
        if (filepath == '~') {
            return std::filesystem::current_path().string();
            } else if (filepath.substr(0, "~/")) {
                return path.join(std::filesystem::current_path().string(), filepath.substr(2));
                } else if (filepath.substr(0, "~~")) {
                    // Don't expand ~~
                    return filepath;
                    } else {
                        // Handle ~user/path by expanding it to cwd/user/path
                        return path.join(std::filesystem::current_path().string(), filepath.substr(1));
                    }
                }

                return filepath;

}

std::string resolvePgliteDir(std::optional<std::string> dir, std::optional<std::string> fallbackDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envPath = resolveEnvFile();
    if (existsSync(envPath)) {
        dotenv.config({ path: envPath });
    }

    const auto base =;
    dir ||;
    std::getenv("PGLITE_DATA_DIR") ||;
    fallbackDir ||;
    path.join(std::filesystem::current_path().string(), ".eliza", ".elizadb");

    // Automatically migrate legacy path (<cwd>/.elizadb) to new location (<cwd>/.eliza/.elizadb)
    const auto resolved = expandTildePath(base);
    const auto legacyPath = path.join(std::filesystem::current_path().string(), ".elizadb");
    if (resolved == legacyPath) {
        const auto newPath = path.join(std::filesystem::current_path().string(), ".eliza", ".elizadb");
        std::getenv("PGLITE_DATA_DIR") = newPath;
        return newPath;
    }

    return resolved;

}

} // namespace elizaos
