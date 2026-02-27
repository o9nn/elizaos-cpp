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

    // If explicit dir provided, use it
    if (dir) {
        const auto resolved = expandTildePath(dir);
        std::getenv("PGLITE_DATA_DIR") = resolved;
        return resolved;
    }

    // If fallbackDir provided, use it as fallback
    if (fallbackDir && !std::getenv("PGLITE_DATA_DIR") && !std::getenv("ELIZA_DATABASE_DIR")) {
        const auto resolved = expandTildePath(fallbackDir);
        std::getenv("PGLITE_DATA_DIR") = resolved;
        return resolved;
    }

    // Use the centralized path configuration from core
    const auto resolved = getDatabaseDir();

    // Persist chosen root for the process so child modules see it (backward compat)
    std::getenv("PGLITE_DATA_DIR") = resolved;
    return resolved;

}

bool isWebUIEnabled() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto isProduction = std::getenv("NODE_ENV") == "production";
    const auto uiEnabledEnv = std::getenv("ELIZA_UI_ENABLE");

    // Treat empty strings as std::nullopt
    if (uiEnabledEnv != std::nullopt && uiEnabledEnv != '') {
        return parseBooleanFromText(uiEnabledEnv);
    }

    // Default: enabled in dev, disabled in prod
    return !isProduction;

}

} // namespace elizaos
