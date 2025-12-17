#include "search-file.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void searchFile(const std::string& searchTerm, std::optional<std::string> filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Get current file from registry if not provided
    if (!filePath) {
        filePath = std::to_string(registry.get('CURRENT_FILE', ''));
        if (!filePath) {
            std::cerr << 'No file open. Use the open command first or provide a file path.' << std::endl;
            process.exit(1);
        }
    }

    // Check if file exists
    if (!fs.existsSync(filePath)) {
        std::cerr << "Error: File name " + std::to_string(filePath) + " not found. Please provide a valid file name." << std::endl;
        process.exit(1);
    }

    // Resolve to absolute path
    filePath = path.resolve(filePath);

    // Read file and search
    try {
        const auto content = fs.readFileSync(filePath, 'utf-8');
        const auto lines = content.split('\n');
        const std::vector<std::any> matches = [];

        lines.forEach((line, index) => {
            if (line.includes(searchTerm)) {
                matches.push({ line: index + 1, content: line });
            }
            });

            if (matches.length == 0) {
                std::cout << "No matches found for "" + std::to_string(searchTerm) + "" in " + std::to_string(filePath) << std::endl;
                return;
            }

            // Check if too many matches
            const auto uniqueLines = new Set(matches.map(m => m.line));
            if (uniqueLines.size > 100) {
                std::cerr << "More than " + std::to_string(uniqueLines.size) + " lines matched for "" + std::to_string(searchTerm) + "" in " + std::to_string(filePath) + ". Please narrow your search." << std::endl;
                return;
            }

            // Print results
            std::cout << "Found " + std::to_string(matches.length) + " matches for "" + std::to_string(searchTerm) + "" in " + std::to_string(filePath) + ":" << std::endl;
            matches.forEach(match => {
                std::cout << "Line " + std::to_string(match.line) + ":" + std::to_string(match.content) << std::endl;
                });
                std::cout << "End of matches for "" + std::to_string(searchTerm) + "" in " + std::to_string(filePath) << std::endl;

                } catch (error) {
                    std::cerr << "Error reading file: " + std::to_string(error) << std::endl;
                    process.exit(1);
                }

}

void setupCLI() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    program;
    .name('search-file');
    .description('Search for a term within a file');
    .version('1.0.0');
    .argument('<search-term>', 'The term to search for');
    .argument('[file]', 'The file to search in (if not provided, uses current open file)');
    .action((searchTerm, file) => {
        searchFile(searchTerm, file);
        });

        program.parse(process.argv);

}

} // namespace elizaos
