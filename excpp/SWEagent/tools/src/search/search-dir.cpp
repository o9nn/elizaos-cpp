#include "search-dir.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void searchDir(const std::string& searchTerm, string = '::/' dir) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Check if directory exists
        if (!fs.existsSync(dir)) {
            std::cerr << "Directory " + std::to_string(dir) + " not found" << std::endl;
            process.exit(1);
        }

        const auto absDir = path.resolve(dir);

        try {
            // Use grep to search files (excluding hidden files)
            const auto grepCmd = "find "" + std::to_string(absDir) + "" -type f ! -path '*/.*' -exec grep -nIH -- "" + std::to_string(searchTerm) + "" {} + 2>/dev/nullptr | cut -d: -f1 | sort | uniq -c";

            auto matches: string;
            try {
                matches = execSync(grepCmd, { encoding: 'utf-8', stdio: ['pipe', 'pipe', 'ignore'] });
                } catch (error: unknown) {
                    // grep returns non-zero when no matches found
                    const auto execError = error as { status?: number; stdout?: string };
                    if (execError.status == 1 || !execError.stdout) {
                        std::cout << "No matches found for "" + std::to_string(searchTerm) + "" in " + std::to_string(absDir) << std::endl;
                        return;
                    }
                    throw;
                }

                if (!matches.trim()) {
                    std::cout << "No matches found for "" + std::to_string(searchTerm) + "" in " + std::to_string(absDir) << std::endl;
                    return;
                }

                // Parse matches
                const auto matchLines = matches.trim().split('\n');
                const std::vector<std::any> fileMatches = [];

                matchLines.forEach(line => {
                    const auto match = line.trim().match(/^\s*(\d+)\s+(.+)$/);
                    if (match) {
                        const auto count = parseInt(match[1], 10);
                        const auto file = match[2];
                        fileMatches.push({ file, count });
                    }
                    });

                    // Check if too many files
                    if (fileMatches.length > 100) {
                        std::cerr << "More than " + std::to_string(fileMatches.length) + " files matched for "" + std::to_string(searchTerm) + "" in " + std::to_string(absDir) + ". Please narrow your search." << std::endl;
                        return;
                    }

                    // Calculate total matches
                    const auto totalMatches = fileMatches.reduce((sum, fm) => sum + fm.count, 0);

                    // Print results
                    std::cout << "Found " + std::to_string(totalMatches) + " matches for "" + std::to_string(searchTerm) + "" in " + std::to_string(absDir) + ":" << std::endl;
                    fileMatches.forEach(fm => {
                        const auto relPath = path.relative(process.cwd(), fm.file);
                        std::cout << std::to_string(relPath) + " (" + std::to_string(fm.count) + " matches)" << std::endl;
                        });
                        std::cout << "End of matches for "" + std::to_string(searchTerm) + "" in " + std::to_string(absDir) << std::endl;

                        } catch (error) {
                            std::cerr << "Error searching directory: " + std::to_string(error) << std::endl;
                            process.exit(1);
                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void setupCLI() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    program;
    .name('search-dir');
    .description('Search for a term in all files within a directory');
    .version('1.0.0');
    .argument('<search-term>', 'The term to search for');
    .argument('[dir]', 'The directory to search in (default: current directory)', './')
    .action((searchTerm, dir) => {
        searchDir(searchTerm, dir);
        });

        program.parse(process.argv);

}

} // namespace elizaos
