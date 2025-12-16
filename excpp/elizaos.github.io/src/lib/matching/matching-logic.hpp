#include "types.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Escapes special characters in a string for use in a regular expression.
 *
 * @param str The string to escape.
 * @returns The escaped string.
 */
std::string escapeRegExp(const std::string& str);

/**
 * Performs simple string matching.
 *
 * @param pattern The string pattern to search for.
 * @param content The content string to search within.
 * @param options Matching options.
 * @returns True if the pattern is found in the content according to options, false otherwise.
 */
bool matchString(const std::string& pattern, const std::string& content, PatternOptions options);

/**
 * Performs regular expression matching.
 *
 * @param pattern The regex pattern string.
 * @param content The content string to test against the regex.
 * @param options Matching options (caseSensitive, wordBoundary).
 * @returns True if the content matches the regex according to options, false otherwise.
 */
bool matchRegex(const std::string& pattern, const std::string& content, PatternOptions options);

/**
 * Performs glob pattern matching (typically for file paths).
 *
 * @param pattern The glob pattern string.
 * @param content The content string (usually a file path) to match against the glob.
 * @param options Matching options (caseSensitive). Word boundary is ignored.
 * @returns True if the content matches the glob pattern according to options, false otherwise.
 */
bool matchGlob(const std::string& pattern, const std::string& content, PatternOptions options);

} // namespace elizaos
