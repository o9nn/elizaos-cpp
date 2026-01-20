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
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Formats the names of evaluators into a comma-separated list, each enclosed in single quotes.
 * @param evaluators - An array of evaluator objects.
 * @returns A string that concatenates the names of all evaluators, each enclosed in single quotes and separated by commas.
 */
/**
 * Formats the names of the evaluators in the provided array.
 *
 * @param {Evaluator[]} evaluators - Array of evaluators.
 * @returns {string} - Formatted string of evaluator names.
 */
void formatEvaluatorNames(const std::vector<Evaluator>& evaluators);

/**
 * Formats evaluator examples into a readable string, replacing placeholders with generated names.
 * @param evaluators - An array of evaluator objects, each containing examples to format.
 * @returns A string that presents each evaluator example in a structured format, including context, messages, and outcomes, with placeholders replaced by generated names.
 */
void formatEvaluatorExamples(const std::vector<Evaluator>& evaluators);

/**
 * Formats evaluator details into a string, including both the name and description of each evaluator.
 * @param evaluators - An array of evaluator objects.
 * @returns A string that concatenates the name and description of each evaluator, separated by a colon and a newline character.
 */
void formatEvaluators(const std::vector<Evaluator>& evaluators);

    // Get evaluators that validate for this message

    // Wait for all validations

    // Filter out null values

    // Format evaluator-related texts

    // Combine all text sections


} // namespace elizaos
