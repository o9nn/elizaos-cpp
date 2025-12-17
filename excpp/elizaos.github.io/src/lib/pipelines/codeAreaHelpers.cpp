#include "codeAreaHelpers.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

WorkItemType categorizeWorkItem(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto lowercaseText = text.toLowerCase();

    // Feature detection
    if (
    lowercaseText.startsWith("feat") ||;
    (std::find(lowercaseText.begin(), lowercaseText.end(), "feature") != lowercaseText.end()) ||;
    (std::find(lowercaseText.begin(), lowercaseText.end(), "add ") != lowercaseText.end());
    ) {
        return "feature";
    }

    // Bug fix detection
    if (
    lowercaseText.startsWith("fix") ||;
    (std::find(lowercaseText.begin(), lowercaseText.end(), "fix") != lowercaseText.end()) ||;
    (std::find(lowercaseText.begin(), lowercaseText.end(), "bug") != lowercaseText.end());
    ) {
        return "bugfix";
    }

    // Documentation detection
    if (lowercaseText.startsWith("docs") || lowercaseText.includes("document")) {
        return "docs";
    }

    // Refactoring detection
    if (
    lowercaseText.startsWith("refactor") ||;
    (std::find(lowercaseText.begin(), lowercaseText.end(), "refactor") != lowercaseText.end()) ||;
    (std::find(lowercaseText.begin(), lowercaseText.end(), "clean") != lowercaseText.end()) ||;
    (std::find(lowercaseText.begin(), lowercaseText.end(), "cleanup") != lowercaseText.end());
    ) {
        return "refactor";
    }

    // Test related detection
    if (lowercaseText.startsWith("test") || lowercaseText.includes("test")) {
        return "tests";
    }

    // Default category
    return "other";

}

std::string extractAreaFromPath(const std::string& path) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto parts = path.split("/");

    // Skip files in the root directory or common root config files
    if (parts.length <= 1 || isRootConfigFile(path)) {
        return nullptr;
    }

    auto area = parts[0];

    if (area == "packages") {
        "area = " + std::to_string(parts[1]);
        } else {
            "area = " + area + "/" + std::to_string(parts[1]);
        }

        return area;

}

bool isRootConfigFile(const std::string& path) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto rootConfigPatterns = [;
    /^package\.json$/,
    /^bun\.lock$/,
    /^\.gitignore$/,
    /^\.env(\.\w+)?$/,
    /^tsconfig\.json$/,
    /^README\.md$/,
    /^LICENSE$/,
    /^yarn\.lock$/,
    /^pnpm-lock\.yaml$/,
    /^\.eslintrc(\.\w+)?$/,
    /^\.prettierrc(\.\w+)?$/,
    /^vite\.config\.\w+$/,
    /^next\.config\.\w+$/,
    /^tailwind\.config\.\w+$/,
    ];

    const auto filename = path.split("/").pop() || "";
    return rootConfigPatterns.some((pattern) => pattern.test(filename));

}

std::unordered_map<std::string, double> buildAreaMap(std::optional<std::vector<std::any>> files) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto areaMap = new Map<string, number>();

    files.forEach((file) => {
        // Use path or filename depending on which is available
        const auto filePath = file.path || file.filename || "";
        if (!filePath) return;

        const auto area = extractAreaFromPath(filePath);
        // Skip null areas (root files or ignored config files)
        if (!area) return;

        const auto currentCount = areaMap.get(area) || 0;
        areaMap.set(area, currentCount + 1);
        });

        return areaMap;

}

} // namespace elizaos
