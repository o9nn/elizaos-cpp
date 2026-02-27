#include "validation.hpp"
#include <future>
#include <iostream>
#include <stdexcept>

namespace elizaos {

CreateOptions validateCreateOptions(const std:& options) {
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

void validateProjectName(const std:& name) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    isValid; error?: std:
}

void processPluginName(const std:& name) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    isValid;
    processedName?: std:;
    error?: std:;

}

std::future<> validateTargetDirectory(const std:& targetDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    isValid; error?: std:
}

} // namespace elizaos
