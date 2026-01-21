#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string expandTildePath(const std::string& filepath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!filepath) {
        return filepath;
    }

    if (filepath.startsWith('~')) {
        if (filepath == '~') {
            return process.cwd();
            } else if (filepath.startsWith("~/")) {
                return path.join(process.cwd(), filepath.slice(2));
                } else if (filepath.startsWith("~~")) {
                    // Don't expand ~~
                    return filepath;
                    } else {
                        // Handle ~user/path by expanding it to cwd/user/path
                        return path.join(process.cwd(), filepath.slice(1));
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
        process.env.PGLITE_DATA_DIR = resolved;
        return resolved;
    }

    // If fallbackDir provided, use it as fallback
    if (fallbackDir && !process.env.PGLITE_DATA_DIR && !process.env.ELIZA_DATABASE_DIR) {
        const auto resolved = expandTildePath(fallbackDir);
        process.env.PGLITE_DATA_DIR = resolved;
        return resolved;
    }

    // Use the centralized path configuration from core
    const auto resolved = getDatabaseDir();

    // Persist chosen root for the process so child modules see it (backward compat)
    process.env.PGLITE_DATA_DIR = resolved;
    return resolved;

}

bool isWebUIEnabled() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto isProduction = process.env.NODE_ENV == "production";
    const auto uiEnabledEnv = process.env.ELIZA_UI_ENABLE;

    // Treat empty strings as undefined
    if (uiEnabledEnv != undefined && uiEnabledEnv.trim() != '') {
        return parseBooleanFromText(uiEnabledEnv);
    }

    // Default: enabled in dev, disabled in prod
    return !isProduction;

}

} // namespace elizaos
