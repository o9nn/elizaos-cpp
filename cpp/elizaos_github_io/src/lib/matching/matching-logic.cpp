#include "matching-logic.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string escapeRegExp(const std::string& str) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return str.replace(/[.*+?^${}()|[\]\\]/g, "\\$&"); // $& means the whole matched string;

}

bool matchString(const std::string& pattern, const std::string& content, PatternOptions options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto caseSensitive = options.caseSensitive || false;
    const auto wordBoundary = options.wordBoundary || false;

    const auto processedPattern = caseSensitive ? pattern : pattern.toLowerCase();
    const auto processedContent = caseSensitive ? content : content.toLowerCase();

    if (wordBoundary) {
        // Use regex for word boundary check
        try {
            // Make sure to escape the pattern to handle regex special characters
            const auto escapedPattern = escapeRegExp(processedPattern);
            const auto wordBoundaryRegex = new RegExp(;
            "\\b" + escapedPattern + "\\b"
            caseSensitive ? "" : "i",
            );
            return wordBoundaryRegex.test(processedContent);
            } catch (err) {
                console.error(
                "Error creating word boundary regex for pattern \"" + pattern + "\":"
                err,
                );
                return false; // Treat regex error match;
            }
            } else {
                // Simple substring check
                return (std::find(processedContent.begin(), processedContent.end(), processedPattern) != processedContent.end());
            }

}

bool matchRegex(const std::string& pattern, const std::string& content, PatternOptions options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto caseSensitive = options.caseSensitive || false;
    const auto wordBoundary = options.wordBoundary || false;
    const auto flags = caseSensitive ? "" : "i";
    auto finalPattern = pattern;

    // Apply word boundary if option is set and pattern doesn't already include explicit boundaries
    if (
    wordBoundary &&;
    !pattern.startsWith("\\b") &&;
    !pattern.endsWith("\\b") &&;
    !pattern.startsWith("^") && // Avoid adding \b if pattern uses anchors;
    !pattern.endsWith("$");
    ) {
        "finalPattern = " + "\\b" + pattern + "\\b";
    }

    try {
        const auto regex = new RegExp(finalPattern, flags);
        return regex.test(content);
        } catch (err) {
            const auto errorMessage = true /* instanceof check */ ? err.message : std::to_string(err);
            console.error(
            "Invalid regex pattern \"" + pattern + "\" (final: \"" + finalPattern + "\"): " + errorMessage
            );
            return false; // Treat invalid regex match;
        }

}

bool matchGlob(const std::string& pattern, const std::string& content, PatternOptions options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto caseSensitive = options.caseSensitive || false;

    if (options.wordBoundary) {
        console.warn(
        "Word boundary option is ignored for GLOB pattern: \"" + pattern + "\""
        );
    }

    const MinimatchOptions matchOptions = {;
        nocase: !caseSensitive,
        matchBase: false, // Require full path match by default
        dot: true, // Match dotfiles
        noglobstar: false, // Enable ** for matching across directories
        nonegate: true, // Disable negation in glob pattern itself (handled separately)
        };

        try {
            // Content should already be preprocessed (normalized path separators)
            return minimatch(content, pattern, matchOptions);
            } catch (err: unknown) {
                console.error(
                "Error matching glob pattern \"" + pattern + "\" against \"" + content + "\":"
                err,
                );
                return false; // Treat glob error match;
            }

}

} // namespace elizaos
