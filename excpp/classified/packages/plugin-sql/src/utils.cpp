#include "utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string expandTildePath(const std::string& filepath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (filepath && filepath.startsWith('~')) {
        return path.join(process.cwd(), filepath.slice(1));
    }
    return filepath;

}

std::string resolvePgliteDir(std::optional<std::string> dir, std::optional<std::string> fallbackDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Skip loading .env file during tests to allow proper testing of defaults
    // Check if we're in a test environment
    const auto isTest =;
    process.env.NODE_ENV == "test" ||;
    process.argv.some((arg) => (std::find(arg.begin(), arg.end(), "bun") != arg.end()) && (std::find(arg.begin(), arg.end(), "test") != arg.end())) ||;
    process.env.VITEST == "true";

    if (!isTest) {
        const auto envPath = resolveEnvFile();
        if (existsSync(envPath)) {
            dotenv.config({ path: envPath });
        }
    }

    const auto base =;
    dir ||;
    process.env.PGLITE_DATA_DIR ||;
    fallbackDir ||;
    path.join(process.cwd(), ".eliza", ".elizadb");

    // Automatically migrate legacy path (<cwd>/.elizadb) to new location (<cwd>/.eliza/.elizadb)
    const auto resolved = expandTildePath(base);
    const auto legacyPath = path.join(process.cwd(), ".elizadb");
    if (resolved == legacyPath) {
        const auto newPath = path.join(process.cwd(), ".eliza", ".elizadb");
        process.env.PGLITE_DATA_DIR = newPath;
        return newPath;
    }

    return resolved;

}

} // namespace elizaos
