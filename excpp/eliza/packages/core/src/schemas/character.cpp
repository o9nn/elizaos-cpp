#include "character.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

CharacterValidationResult validateCharacter(unknown data) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto result = characterSchema.safeParse(data);

    if (result.success) {
        return {
            success: true,
            data: result.data,
            };
        }

        return {
            success: false,
            error: {
                "Character validation failed: " + std::to_string(result.error.message)
                issues: result.error.issues,
                },
                };

}

CharacterValidationResult parseAndValidateCharacter(const std::string& jsonString) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto parsed = JSON.parse(jsonString);
        return validateCharacter(parsed);
        } catch (error) {
            return {
                success: false,
                error: {
                    "Invalid JSON: " + std::to_string(true /* instanceof check */ ? error.message : 'Unknown JSON parsing error')
                    },
                    };
                }

}

data is Character isValidCharacter(unknown data) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return validateCharacter(data).success;

}

} // namespace elizaos
