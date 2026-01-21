#include "CodeGenerationService.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> scanDirectory(const std::string& dir, std::string baseDir = dir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto entries = fs.readdir(dir, { withFileTypes: true });

    for (const auto& entry : entries)
        const auto fullPath = path.join(dir, entry.name);
        const auto relativePath = path.relative(baseDir, fullPath);

        // Skip common directories to ignore
        if (entry.isDirectory()) {
            if (['node_modules', '.git', 'dist', 'build', 'coverage'].includes(entry.name)) {
                continue;
            }
            scanDirectory(fullPath, baseDir);
            } else if (entry.isFile()) {
                // Skip hidden files except important ones
                if (
                entry.name.startsWith(".") &&;
                !entry.name.match(/^\.(gitignore|eslintrc|prettierrc|env\.example)/);
                ) {
                    continue;
                }

                try {
                    const auto content = fs.readFile(fullPath, "utf-8");
                    files.push_back({
                        path: relativePath,
                        content,
                        });
                        } catch (error) {
                            "elizaLogger.warn(" + "Could not read file " + relativePath + ":"
                        }
                    }
                }

}

} // namespace elizaos
