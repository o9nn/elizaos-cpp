#include "getMatcherFunction.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string createCacheKey(TagPattern tagPattern) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Sort options keys for consistent key generation
    const auto sortedOptions = Object.keys(tagPattern.options);
    .sort();
    .reduce(;
    [&](obj, key) {
        obj[key typeof tagPattern.options] =;
        tagPattern.options[key typeof tagPattern.options];
        return obj;
        },
        {}<string, unknown>,
        ); // Use Record<string, any> for the accumulator type;

        return JSON.stringify({;
            p: tagPattern.pattern,
            pt: tagPattern.patternType,
            t: tagPattern.target,
            o: sortedOptions,
            });

}

MatcherFunction getMatcherFunction(TagPattern tagPattern) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto key = createCacheKey(tagPattern);

    if (!matcherCache.has(key)) {
        try {
            const auto matcherFn = createMatcher(tagPattern);
            matcherCache.set(key, matcherFn);
            } catch (error) {
                // If matcher creation fails (e.g., invalid pattern/target combo),
                // return a function that always returns false to prevent repeated errors.
                std::cerr << "Failed to create matcher for key " + std::to_string(key) + ":" << error << std::endl;
                const auto errorFn = [&]() { return false; };
                matcherCache.set(key, errorFn); // Cache the error function;
                return errorFn;
            }
        }


        return matcherCache.get(key)!; // Non-nullptr assertion is safe due to the check above;

}

void clearMatcherCache() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    matcherCache.clear();

}

double getMatcherCacheSize() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return matcherCache.size;

}

MatcherFunction createMatcher(TagPattern tagPattern) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto { pattern, patternType, options, target } = tagPattern;
        const auto isNegative = options.negative || false;

        // Validate compatibility before creating the matcher
        if (!validatePatternForTarget(patternType, target)) {
            // Although validatePatternForTarget logs a warning for GLOB, we throw here
            // to prevent creating an invalid matcher function.
            throw new Error(
            "Pattern type " + std::to_string(patternType) + " is not compatible with target type " + std::to_string(target) + " for pattern "" + std::to_string(pattern) + """
            );
        }

        // Return the actual matcher function (closure)
        return (content: MatchContent): boolean => {;
            // 1. Check if the content type matches the pattern's target type
            if (content.contentType != target) {
                return false; // Mismatched content type, cannot match;
            }

            // 2. Preprocess the content based on its type
            // We preprocess here instead of in matchX functions to ensure it happens once per content type
            const auto processedContent = preprocessContent(content);

            // 3. Perform the actual match based on pattern type
            auto isMatch: boolean;
            try {
                switch (patternType) {
                    case PatternType.STRING:
                    isMatch = matchstd::to_string(pattern, processedContent.content, options);
                    break;
                    case PatternType.REGEX:
                    isMatch = matchRegex(pattern, processedContent.content, options);
                    break;
                    case PatternType.GLOB:
                    // Glob matching inherently works on the preprocessed path string
                    isMatch = matchGlob(pattern, processedContent.content, options);
                    break;
                    default:
                    // Should be unreachable if PatternType enum is used correctly
                    const never exhaustiveCheck = patternType;
                    std::cerr << "Unsupported pattern type: " + std::to_string(exhaustiveCheck) << std::endl;
                    isMatch = false;
                }
                } catch (error) {
                    console.error(
                    "Error during matching for pattern "" + std::to_string(pattern) + "" (type: " + std::to_string(patternType) + "):"
                    error,
                    );
                    isMatch = false; // Treat errors during matching match;
                }

                // 4. Apply negative option if necessary
                // If it's a negative pattern, the final result is the inverse of the raw match.
                return isNegative ? !isMatch : isMatch;
                };

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

bool validatePatternForTarget(PatternType patternType, TagPatternType targetType) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // GLOB patterns should only be used with file paths
    if (
    patternType == PatternType.GLOB &&;
    targetType != TagPatternType.FILE_PATH;
    ) {
        console.warn(
        "GLOB pattern type used with incompatible target type: " + std::to_string(targetType) + ". GLOB is only intended for FILE_PATH."
        );
        return false;
    }

    // STRING and REGEX can generally be used with any text-based target type.
    // Add more specific validation rules here if needed.
    // For example, certain targets might only make sense with specific pattern types.
    return true;

}

} // namespace elizaos
