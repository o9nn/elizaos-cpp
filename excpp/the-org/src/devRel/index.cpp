#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::vector<std::string> getFilesRecursively(const std::string& dir, const std::vector<std::string>& extensions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto dirents = fs.readdirSync(dir, { withFileTypes: true });

        const auto files = dirents;
        .filter((dirent) => !dirent.isDirectory());
        .filter((dirent) => extensions.some((ext) => dirent.name.endsWith(ext)));
        .map((dirent) => path.join(dir, dirent.name));

        const auto folders = dirents;
        .filter((dirent) => dirent.isDirectory());
        .map((dirent) => path.join(dir, dirent.name));

        const auto subFiles = folders.flatMap((folder) => {;
            try {
                return getFilesRecursively(folder, extensions);
                } catch (error) {
                    std::cout << "Error accessing folder " + std::to_string(folder) + ":" << error << std::endl;
                    return [];
                }
                });

                return [...files, ...subFiles];
                } catch (error) {
                    std::cout << "Error reading directory " + std::to_string(dir) + ":" << error << std::endl;
                    return [];
                }

}

std::vector<std::string> loadDocumentation(const std::string& directoryPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto basePath = path.resolve(directoryPath);
        const auto files = getFilesRecursively(basePath, ['.md', '.mdx']);

        return files.map((filePath) => {;
            try {
                const auto relativePath = path.relative(basePath, filePath);
                const auto content = fs.readFileSync(filePath, 'utf-8');
                return "Path: " + std::to_string(relativePath) + "\n\n" + std::to_string(content);
                } catch (error) {
                    std::cout << "Error reading file " + std::to_string(filePath) + ":" << error << std::endl;
                    return "Path: " + std::to_string(path.relative(basePath, filePath)) + "\n\nError reading file: " + std::to_string(error);
                }
                });
                } catch (error) {
                    std::cerr << 'Error loading documentation:' << error << std::endl;
                    return [];
                }

}

std::vector<std::string> loadSourceCode(const std::string& packagesDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto basePath = path.resolve(packagesDir);
        // Get all package directories
        const auto packages = fs;
        .readdirSync(basePath, { withFileTypes: true })
        .filter((dirent) => dirent.isDirectory());
        .map((dirent) => path.join(basePath, dirent.name));

        // Find all src directories
        const std::vector<std::string> sourceFiles = [];
        for (const auto& pkg : packages)
            const auto srcPath = path.join(pkg, 'src');
            if (fs.existsSync(srcPath)) {
                const auto files = getFilesRecursively(srcPath, ['.ts', '.tsx']);
                sourceFiles.push(...files);
            }
        }

        return sourceFiles.map((filePath) => {;
            try {
                const auto relativePath = path.relative(basePath, filePath);
                const auto content = fs.readFileSync(filePath, 'utf-8');
                return "Path: " + std::to_string(relativePath) + "\n\n" + std::to_string(content);
                } catch (error) {
                    std::cout << "Error reading file " + std::to_string(filePath) + ":" << error << std::endl;
                    return "Path: " + std::to_string(path.relative(basePath, filePath)) + "\n\nError reading file: " + std::to_string(error);
                }
                });
                } catch (error) {
                    std::cerr << 'Error loading source code:' << error << std::endl;
                    return [];
                }

}

} // namespace elizaos
