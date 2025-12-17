#include "flake8-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatFlake8Output(const std::string& currentErrors, const std::string& previousErrorsString, [number replacementWindow, double replacementNLines, bool showLineNumbers = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!currentErrors.trim()) {
        return '';
    }

    // Parse current errors
    const auto currentErrorsList = parseFlake8Output(currentErrors);
    const auto previousErrorsList = parseFlake8Output(previousErrorsString);

    // Update previous errors based on replacement window
    const auto updatedPreviousErrors = updatePreviousErrors(previousErrorsList, replacementWindow, replacementNLines);

    // Find new errors (errors in current that aren't in updated previous)
    const auto newErrors = findNewErrors(currentErrorsList, updatedPreviousErrors);

    // Format output
    if (newErrors.length == 0) {
        return '';
    }

    return formatErrorsForDisplay(newErrors, showLineNumbers);

}

std::vector<Flake8ErrorClass> parseFlake8Output(const std::string& output) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!output.trim()) {
        return [];
    }

    const auto lines = output.trim().split('\n');
    const std::vector<Flake8ErrorClass> errors = [];

    for (const auto& line : lines)
        if (line.trim()) {
            try {
                errors.push(Flake8ErrorClass.fromLine(line));
                } catch {
                    // Skip invalid lines
                }
            }
        }

        return errors;

}

std::vector<Flake8ErrorClass> updatePreviousErrors(const std::vector<Flake8ErrorClass>& previousErrors, [number replacementWindow, double replacementNLines) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [startLine, endLine] = replacementWindow;
    const auto linesDeleted = endLine - startLine + 1;
    const auto linesDelta = replacementNLines - linesDeleted;

    const std::vector<Flake8ErrorClass> updated = [];

    for (const auto& error : previousErrors)
        // Skip errors within the replacement window
        if (error.line >= startLine && error.line <= endLine) {
            continue;
        }

        // Adjust line numbers for errors after the replacement
        if (error.line > endLine) {
            updated.push(new Flake8ErrorClass(error.file, error.line + linesDelta, error.column, error.message));
            } else {
                updated.push(error);
            }
        }

        return updated;

}

std::vector<Flake8ErrorClass> findNewErrors(const std::vector<Flake8ErrorClass>& currentErrors, const std::vector<Flake8ErrorClass>& previousErrors) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto previousSet = new Set(previousErrors.map((e) => e.toString()));

    return currentErrors.filter((error) => !previousSet.has(error.toString()));

}

std::string formatErrorsForDisplay(const std::vector<Flake8ErrorClass>& errors, bool showLineNumbers) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (errors.length == 0) {
        return '';
    }

    return errors;
    .map((error) => {
        if (showLineNumbers) {
            return "- line " + std::to_string(error.line) + " col " + std::to_string(error.column) + ": " + std::to_string(error.message);
        }
        return "- " + std::to_string(error.message);
        });
        .join('\n');

}

} // namespace elizaos
