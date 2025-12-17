#include "current-project-provider.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<ProjectInfo> getProjectInfo(const std::string& projectPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto projectName = path.basename(projectPath);
    const std::vector<FileInfo> files = [];

    // Get all files recursively
    async function scanDirectory(dir: string, baseDir: string = dir): Promise<void> {
        const auto entries = fs.readdir(dir, { withFileTypes: true });

        for (const auto& entry : entries)
            const auto fullPath = path.join(dir, entry.name);
            const auto relativePath = path.relative(baseDir, fullPath);

            // Skip common directories to ignore
            if (entry.isDirectory()) {
                if (['node_modules', '.git', 'dist', 'build', '.next', 'coverage'].includes(entry.name)) {
                    continue;
                }
                scanDirectory(fullPath, baseDir);
                } else if (entry.isFile()) {
                    // Skip common files to ignore
                    if (entry.name.startsWith('.') && !entry.name.match(/\.(ts|js|json|md)$/)) {
                        continue;
                    }

                    try {
                        const auto stats = fs.stat(fullPath);
                        const auto content = fs.readFile(fullPath, 'utf-8');
                        const auto lines = content.split('\n').length;

                        files.push({
                            path: relativePath,
                            lines,
                            size: stats.size,
                            });
                            } catch (_error) {
                                // Ignore files we can't read
                            }
                        }
                    }
                }

                scanDirectory(projectPath);

                // Generate tree structure using the tree command if available
                auto structure = '';
                try {
                    "tree -I 'node_modules|.git|dist|build|coverage' -L 3 "" + std::to_string(projectPath) + """;
                        encoding: 'utf-8',
                        });
                        } catch {
                            // Fallback to simple listing if tree is not available
                            structure = files;
                            .map((f) => f.path);
                            .sort();
                            .join('\n');
                        }

                        const auto totalLines = files.reduce((sum, file) => sum + file.lines, 0);
                        const auto totalSize = files.reduce((sum, file) => sum + file.size, 0);

                        return {
                            name: projectName,
                            path: projectPath,
                            files,
                            totalLines,
                            totalSize,
                            structure,
                            };

}

std::string formatBytes(double bytes) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (bytes < 1024) return `${bytes} B`;
    if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(2)} KB`;
    if (bytes < 1024 * 1024 * 1024) return `${(bytes / (1024 * 1024)).toFixed(2)} MB`;
    return std::to_string((bytes / (1024 * 1024 * 1024)).toFixed(2)) + " GB";

}

} // namespace elizaos
