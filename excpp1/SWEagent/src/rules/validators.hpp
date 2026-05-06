#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "general.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Validation utilities rules
 */

/**
 * Result of a validation check
 */
struct ValidationResult {
    bool valid;
    std::optional<std::string> file;
    std::vector<Violation> violations;
    std::vector<std::string> warnings;
};

struct Violation {
    std::string rule;
    std::optional<double> line;
    std::optional<double> column;
    std::string message;
    std::variant<'error', 'warning'> severity;
};

/**
 * Validate Python code against rules
 */
class PythonValidator {
public:
    PythonValidator(CodingRule[] = PYTHON_CODING_RULES _rules);
    ValidationResult validate(const std::string& code, std::optional<std::string> filePath);
    bool hasTypeAnnotations(const std::string& code);
    double getLineNumber(const std::string& code, double index);
    bool isMainScript(const std::string& filePath);
    double calculateCommentDensity(const std::string& code);
};

/**
 * Validate TypeScript code against rules
 */
class TypeScriptValidator {
  constructor(_rules: CodingRule[] = TYPESCRIPT_CODING_RULES) {
    // Allow custom rules to be passed in but not used internally yet
  }

    // Check for any type usage

    // Check for missing return types

    // Check for synchronous fs usage

    // Check for JSDoc on exported functions

    // Check comment density

/**
 * Factory function to get appropriate validator
 */
std::variant<PythonValidator, TypeScriptValidator> getValidator(const std::variant<'python', 'typescript'>& language);

/**
 * Validate a file based on its extension
 */
std::future<ValidationResult> validateFile(const std::string& filePath);

/**
 * Validate multiple files
 */

/**
 * Format validation results for display
 */
std::string formatValidationResults(const std::vector<ValidationResult>& results);

} // namespace elizaos
