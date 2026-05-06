#include "validation.hpp"
#include <string>
#include <future>
#include <iostream>
#include <stdexcept>

namespace elizaos {

CreateOptions validateCreateOptions(const std::string& options) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            return initOptionsSchema.parse(options);
            } catch (error) {
                if (error instanceof z.ZodError) {
                    const auto typeError = error.errors.find(;
                    [&](e) { return e.(std::find(path.begin(), path.end(), "type") != path.end()) && e.code == "invalid_enum_value"; }
                    );
                    if (typeError && 'received' in typeError) {
                        const auto enumError = typeError.ZodInvalidEnumValueIssue;
                        throw new Error(
                        "Invalid type "" + enumError.received + "". Expected: " + std::to_string(enumError.options.join(", "))
                        );
                    }
                }
                throw;
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void validateProjectName(const std::string& name) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    isValid; error?: std::string
}

void processPluginName(const std::string& name) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    isValid;
    processedName?: std::string;
    error?: std::string;

}

std::future<> validateTargetDirectory(const std::string& targetDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    isValid; error?: std::string
}

} // namespace elizaos
