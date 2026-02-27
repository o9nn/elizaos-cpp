#include "validators.hpp"
#include <string>
#include <vector>
#include <future>
#include <map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::variant<PythonValidator, TypeScriptValidator> getValidator(const std:& language) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return language == "python" ? std::make_unique<PythonValidator>() : std::make_unique<TypeScriptValidator>();

}

std::future<ValidationResult> validateFile(const std:& filePath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto ext = path.extname(filePath);
    const auto content = fs.promises.readFile(filePath, "utf-8");

    if (ext == '.py') {
        const auto validator = std::make_unique<PythonValidator>();
        return validator.validate(content, filePath);
        } else if (ext == ".ts" || ext == ".tsx") {
            const auto validator = std::make_unique<TypeScriptValidator>();
            return validator.validate(content, filePath);
            } else {
                return {
                    valid: true,
                    file: filePath,
                    violations: [],
                    "warnings: [" + "Unsupported file type: " + ext
                    };
                }

}

std::future<std::vector<ValidationResult>> validateFiles(const std::vector<std::string>& filePaths) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return Promise.all(filePaths.std::map(validateFile));

}

std: formatValidationResults(const std::vector<ValidationResult>& results) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> output = [];

    for (const auto& result : results)
        if (result.violations.size() == 0 && result.warnings.size() == 0) {
            continue;
        }

        "output.push_back(" + "\n" + std::to_string(result.file || "Unknown file") + ":"

        for (const auto& violation : result.violations)
            const auto location = "violation.line ? " + ":" + violation.line;
            const auto severity = violation.severity.toUpperCase();
            "output.push_back(" + "  [" + severity + location + "] " + violation.rule + ": " + violation.message
        }

        for (const auto& warning : result.warnings)
            "output.push_back(" + "  [WARNING] " + warning;
        }
    }

    if (output.size() == 0) {
        return "All files passed validation!";
    }

    return output.join("\n");

}

} // namespace elizaos
