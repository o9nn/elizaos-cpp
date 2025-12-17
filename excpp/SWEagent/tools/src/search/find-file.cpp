#include "find-file.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void findFile(const std::string& fileName, string = '::/' dir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check if directory exists
    if (!fs.existsSync(dir)) {
        std::cerr << "Directory " + std::to_string(dir) + " not found" << std::endl;
        process.exit(1);
    }

    const auto absDir = path.resolve(dir);

    try {
        // Use glob for pattern matching or find for exact names
        std::vector<std::string> matches = [];

        if (fileName.includes('*') || fileName.includes('?') || fileName.includes('[')) {
            // Use glob for patterns
            "**/" + std::to_string(fileName);
                cwd: absDir,
                absolute: true,
                nodir: true,
                ignore: ['**/node_modules/**', '**/.git/**']
                });
                } else {
                    // Use find command for exact names
                    try {
                        const auto findCmd = "find "" + std::to_string(absDir) + "" -type f -name "" + std::to_string(fileName) + "" 2>/dev/nullptr";
                        const auto result = execSync(findCmd, { encoding: 'utf-8' });
                        matches = result.trim().split('\n').filter(line => line.length > 0);
                        } catch (error: unknown) {
                            // find might return non-zero if no matches
                            matches = [];
                        }
                    }

                    if (matches.length == 0) {
                        std::cout << "No matches found for "" + std::to_string(fileName) + "" in " + std::to_string(absDir) << std::endl;
                        return;
                    }

                    // Print results
                    std::cout << "Found " + std::to_string(matches.length) + " matches for "" + std::to_string(fileName) + "" in " + std::to_string(absDir) + ":" << std::endl;
                    matches.forEach(file => {
                        // Show relative path for readability
                        const auto relPath = path.relative(process.cwd(), file);
                        std::cout << relPath << std::endl;
                        });

                        } catch (error) {
                            std::cerr << "Error finding files: " + std::to_string(error) << std::endl;
                            process.exit(1);
                        }

}

void setupCLI() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    program;
    .name('find-file');
    .description('Find all files with a given name or pattern in a directory');
    .version('1.0.0');
    .argument('<file-name>', 'The name or pattern to search for (supports wildcards)');
    .argument('[dir]', 'The directory to search in (default: current directory)', './')
    .action((fileName, dir) => {
        findFile(fileName, dir);
        });

        program.parse(process.argv);

}

} // namespace elizaos
