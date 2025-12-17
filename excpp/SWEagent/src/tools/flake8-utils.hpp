#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Flake8 utilities for linting and error formatting
 * Converted from Python flake8_utils.py
 */

struct Flake8Error {
    std::string file;
    double line;
    double column;
    std::string message;
};

class Flake8ErrorClass {
public:
    Flake8ErrorClass();
    static Flake8ErrorClass fromLine(const std::string& line);
    std::string toString();
};

/**
 * Format flake8 output, filtering out errors that existed before
 */
std::string formatFlake8Output(const std::string& currentErrors, const std::string& previousErrorsString, [number replacementWindow, double replacementNLines, bool showLineNumbers = false);

/**
 * Parse flake8 output into error objects
 */
std::vector<Flake8ErrorClass> parseFlake8Output(const std::string& output);

/**
 * Update line numbers of previous errors based on replacement
 */
std::vector<Flake8ErrorClass> updatePreviousErrors(const std::vector<Flake8ErrorClass>& previousErrors, [number replacementWindow, double replacementNLines);

/**
 * Find errors that are new (not in the previous errors)
 */
std::vector<Flake8ErrorClass> findNewErrors(const std::vector<Flake8ErrorClass>& currentErrors, const std::vector<Flake8ErrorClass>& previousErrors);

/**
 * Format errors for display
 */
std::string formatErrorsForDisplay(const std::vector<Flake8ErrorClass>& errors, bool showLineNumbers);

} // namespace elizaos
