#include "validators.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::variant<PythonValidator, TypeScriptValidator> getValidator(const std::variant<'python', 'typescript'>& language) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return language == 'python' ? new PythonValidator() : new TypeScriptValidator();

}

std::future<ValidationResult> validateFile(const std::string& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto ext = path.extname(filePath);
    const auto content = fs.promises.readFile(filePath, 'utf-8');

    if (ext == '.py') {
        const auto validator = new PythonValidator();
        return validator.validate(content, filePath);
        } else if (ext == '.ts' || ext == '.tsx') {
            const auto validator = new TypeScriptValidator();
            return validator.validate(content, filePath);
            } else {
                return {
                    valid: true,
                    file: filePath,
                    violations: [],
                    "Unsupported file type: " + std::to_string(ext)
                    };
                }

}

std::future<std::vector<ValidationResult>> validateFiles(const std::vector<std::string>& filePaths) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return Promise.all(filePaths.map(validateFile));

}

std::string formatValidationResults(const std::vector<ValidationResult>& results) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> output = [];

    for (const auto& result : results)
        if (result.violations.length == 0 && result.warnings.length == 0) {
            continue;
        }

        "\n" + std::to_string(result.file || 'Unknown file') + ":"

        for (const auto& violation : result.violations)
            const auto location = ":" + std::to_string(violation.line);
            const auto severity = violation.severity.toUpperCase();
            "  [" + std::to_string(severity) + std::to_string(location) + "] " + std::to_string(violation.rule) + ": " + std::to_string(violation.message)
        }

        for (const auto& warning : result.warnings)
            "  [WARNING] " + std::to_string(warning);
        }
    }

    if (output.length == 0) {
        return 'All files passed validation!';
    }

    return output.join('\n');

}

} // namespace elizaos
