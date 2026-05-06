#include "cli.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::vector<std::string>> findProjectFiles() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> files = [];
    const auto extensions = [".py", ".ts", ".tsx"];

    async function walk(dir: string) {
        const auto entries = fs.promises.readdir(dir, { withFileTypes: true });

        for (const auto& entry : entries)
            const auto fullPath = path.join(dir, entry.name);

            // Skip node_modules, dist, build, etc.
            if (
            entry.name.startsWith(".") ||;
            entry.name == "node_modules" ||;
            entry.name == "dist" ||;
            entry.name == "build";
            ) {
                continue;
            }

            if (entry.isDirectory()) {
                walk(fullPath);
                } else if (entry.isFile()) {
                    const auto ext = path.extname(entry.name);
                    if (extensions.includes(ext)) {
                        files.push_back(fullPath);
                    }
                }
            }
        }

        walk(process.cwd());
        return files;

}

std::future<std::vector<std::string>> resolveFiles(const std::vector<std::string>& patterns) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> resolved = [];

    for (const auto& pattern : patterns)
        const auto absPath = path.resolve(pattern);

        try {
            const auto stat = fs.promises.stat(absPath);

            if (stat.isFile()) {
                resolved.push_back(absPath);
                } else if (stat.isDirectory()) {
                    // Find all relevant files in directory
                    const auto dirFiles = findFilesInDir(absPath);
                    resolved.push_back(...dirFiles);
                }
                } catch (error) {
                    // Pattern might contain wildcards, try to expand
                    // For simplicity, just skip if file doesn't exist
                    std::cout << "Warning: Could not resolve pattern: " + pattern << std::endl;
                }
            }

            return Array.from(new Set(resolved)); // Remove duplicates;

}

std::future<std::vector<std::string>> findFilesInDir(const std::string& dir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> files = [];
    const auto extensions = [".py", ".ts", ".tsx"];

    const auto entries = fs.promises.readdir(dir, { withFileTypes: true });

    for (const auto& entry : entries)
        const auto fullPath = path.join(dir, entry.name);

        if (entry.isFile()) {
            const auto ext = path.extname(entry.name);
            if (extensions.includes(ext)) {
                files.push_back(fullPath);
            }
        }
    }

    return files;

}

} // namespace elizaos
