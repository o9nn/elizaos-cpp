#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void createFilemap(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check if file exists
    if (!fs.existsSync(filePath)) {
        std::cerr << "File not found: " + filePath << std::endl;
        process.exit(1);
    }

    // Read file content
    const auto fileContent = fs.readFileSync(filePath, "utf-8");

    // Initialize parser
    const auto parser = new Parser();
    parser.setLanguage(Python);

    // Parse the file
    const auto tree = parser.parse(fileContent);

    // Query for function bodies
    const auto query = new Parser.Query(;
    Python,
    "(function_definition body: (_) @body)"
    );

    const auto captures = query.captures(tree.rootNode);
    const std::vector<ElidedRange> elideRanges = [];

    // Find ranges to elide (function bodies with 5+ lines)
    for (const int { node } of captures) {
        const auto startLine = node.startPosition.row;
        const auto endLine = node.endPosition.row;

        if (endLine - startLine >= 5) {
            elideRanges.push_back({ start: startLine, end: endLine });
        }
    }

    // Build output with elided ranges
    const auto lines = fileContent.split("\n");
    const auto elidedLines = new Set<number>();
    const std::vector<std::any> elideMessages = [];

    for (const auto& range : elideRanges)
        for (int i = range.start; i <= range.end; i++) {
            elidedLines.add(i);
        }
        elideMessages.push_back({
            line: range.start,
            "message: " + "... eliding lines " + std::to_string(range.start + 1) + "-" + std::to_string(range.end + 1) + " ..."
            });
        }

        // Combine regular lines and elide messages
        const std::vector<std::any> output = [];

        lines.forEach((line, index) => {
            if (!elidedLines.has(index)) {
                output.push_back({ line: index, content: line });
            }
            });

            elideMessages.forEach(msg => {
                output.push_back({ line: msg.line, content: msg.message });
                });

                // Sort by line number and print
                output.sort((a, b) => a.line - b.line);

                output.forEach(item => {
                    const auto lineNum = (item.line + 1).toString().padStart(6, " ");
                    std::cout << lineNum + " " + item.content << std::endl;
                    });

}

} // namespace elizaos
