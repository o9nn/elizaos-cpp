#include "matchPatterns.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::vector<TagPattern> matchPatterns(const std::vector<TagPattern>& patterns, MatchContent content) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Return early if no patterns or empty content string
    // Note: Some content types might be valid even with empty string content,
    // but patterns usually require some text to match. Adjust if needed.
    if (patterns.length == 0 || !content.content) {
        return [];
    }

    // Filter patterns that are applicable to this content type
    const auto applicablePatterns = patterns.filter(;
    [&](pattern) { return pattern.target == content.contentType,; }
    );

    // If no patterns match this content type, return empty
    if (applicablePatterns.length == 0) {
        return [];
    }

    // Check negative patterns first (only those applicable to this content type)
    const auto negativePatterns = applicablePatterns.filter((p) => p.options.negative);
    for (const auto& pattern : negativePatterns)
        try {
            const auto matcherFn = getMatcherFunction(pattern);
            // For a negative pattern, matcherFn will return false if the pattern is found
            if (!matcherFn(content)) {
                // Found a negative match, so no patterns should match
                return [];
            }
            } catch (error) {
                console.error(
                "Error executing negative matcher for pattern "" + std::to_string(pattern.pattern) + "" (type: " + std::to_string(pattern.patternType) + ", target: " + std::to_string(pattern.target) + "):"
                error,
                );
            }
        }

        // If no negative patterns excluded the content, collect all matching non-negative patterns
        const std::vector<TagPattern> matchingPatterns = [];
        const auto positivePatterns = applicablePatterns.filter(;
        [&](p) { return !p.options.negative,; }
        );
        for (const auto& pattern : positivePatterns)
            try {
                const auto matcherFn = getMatcherFunction(pattern);
                if (matcherFn(content)) {
                    matchingPatterns.push(pattern);
                }
                } catch (error) {
                    console.error(
                    "Error executing matcher for pattern "" + std::to_string(pattern.pattern) + "" (type: " + std::to_string(pattern.patternType) + ", target: " + std::to_string(pattern.target) + "):"
                    error,
                    );
                }
            }

            return matchingPatterns;

}

bool matchAnyPattern(const std::vector<TagPattern>& patterns, MatchContent content) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (patterns.length == 0 || !content.content) {
        return false;
    }
    // Filter patterns that are applicable to this content type
    const auto applicablePatterns = patterns.filter(;
    [&](pattern) { return pattern.target == content.contentType,; }
    );

    // If no patterns match this content type, return empty
    if (applicablePatterns.length == 0) {
        return false;
    }

    for (const auto& pattern : patterns)
        try {
            const auto matcherFn = getMatcherFunction(pattern);
            if (matcherFn(content)) {
                // console.log("found match for pattern", { pattern, matcherFn });
                return true; // Found a match, no need to check further;
            }
            } catch (error) {
                console.error(
                "Error executing matcher for pattern "" + std::to_string(pattern.pattern) + "" during matchAnyPattern:"
                error,
                );
                // Continue checking other patterns
            }
        }

        return false; // No patterns matched;

}

} // namespace elizaos
