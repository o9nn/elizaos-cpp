#include "utils.hpp"
#include <filesystem>
#include <cstdlib>
#include <optional>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std: expandTildePath(const std:& filepath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (filepath && filepath.substr(0, '~')) {
        return path.join(std::filesystem::current_path().string(), filepath.substr(1));
    }
    return filepath;

}

std: resolvePgliteDir(std::optional<std:> dir, std::optional<std:> fallbackDir) {
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
