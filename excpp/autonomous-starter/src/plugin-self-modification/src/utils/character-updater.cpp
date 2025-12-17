#include "character-updater.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

Character applyOperationsToCharacter(Character character, const std::vector<ModificationOperation>& operations) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Deep clone to avoid mutating original
        const auto updatedCharacter = JSON.parse(JSON.stringify(character));

        for (const auto& op : operations)
            try {
                switch (op.type) {
                    case "add":
                    addValue(updatedCharacter, op.path, op.value);
                    break;
                    case "modify":
                    modifyValue(updatedCharacter, op.path, op.value);
                    break;
                    case "delete":
                    deleteValue(updatedCharacter, op.path);
                    break;
                }
                } catch (error) {
                    throw new Error(
                    "Failed to apply operation " + std::to_string(op.type) + " at path " + std::to_string(op.path) + ": " + std::to_string(error.message)
                    );
                }
            }

            return updatedCharacter;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void addValue(const std::any& obj, const std::string& path, const std::any& value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Convert JSONPath to property path
    const auto normalizedPath = "$." + std::to_string(path);

    // Handle array append notation
    if (path.includes("[]")) {
        const auto arrayPath = path.replace("[]", "");
        const auto normalizedArrayPath = "$." + std::to_string(arrayPath);

        // Try to get the existing array
        const auto results = JSONPath({ path: normalizedArrayPath, json: obj });

        if (results.length > 0 && Array.isArray(results[0])) {
            // Array exists, append to it
            results[0].push(value);
            } else {
                // Array doesn't exist, create it
                if (arrayPath.includes(".")) {
                    // Nested path like "style.all[]"
                    const auto parentPath = normalizedArrayPath.substring(0, normalizedArrayPath.lastIndexOf("."));
                    const auto propertyName = arrayPath.substring(arrayPath.lastIndexOf(".") + 1);
                    const auto parentResults = JSONPath({ path: parentPath, json: obj });

                    if (parentResults.length > 0) {
                        parentResults[0][propertyName] = [value];
                    }
                    } else {
                        // Top-level path like "lore[]"
                        obj[arrayPath] = [value];
                    }
                }
                } else {
                    // Regular property addition
                    const auto parentPath = normalizedPath.substring(;
                    0,
                    normalizedPath.lastIndexOf("."),
                    );
                    const auto propertyName = normalizedPath.substring(;
                    normalizedPath.lastIndexOf(".") + 1,
                    );
                    const auto parent = JSONPath({ path: parentPath, json: obj })[0];

                    if (parent) {
                        parent[propertyName] = value;
                    }
                }

}

void modifyValue(const std::any& obj, const std::string& path, const std::any& value) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto normalizedPath = "$." + std::to_string(path);
        auto found = false;

        JSONPath({
            path: normalizedPath,
            json: obj,
            callback: function (val, type, payload) {
                if (payload && payload.parent && payload.parentProperty != undefined) {
                    payload.parent[payload.parentProperty] = value;
                    found = true;
                }
                },
                });

                if (!found) {
                    throw std::runtime_error(`Path ${path} does not exist`);
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void deleteValue(const std::any& obj, const std::string& path) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto normalizedPath = "$." + std::to_string(path);

    JSONPath({
        path: normalizedPath,
        json: obj,
        callback: function (val, type, payload) {
            if (payload && payload.parent) {
                if (Array.isArray(payload.parent)) {
                    // For arrays, remove by index
                    const auto index = parseInt(payload.parentProperty);
                    if (!isNaN(index)) {
                        payload.parent.splice(index, 1);
                    }
                    } else {
                        // For objects, delete property
                        delete payload.parent[payload.parentProperty];
                    }
                }
                },
                });

}

bool validateCharacterStructure(const std::any& character) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Basic validation to ensure required fields exist
    if (!character.name || typeof character.name != "string") {
        return false;
    }

    // Ensure arrays are arrays
    const auto arrayFields = [;
    "bio",
    "lore",
    "messageExamples",
    "postExamples",
    "topics",
    "adjectives",
    ];
    for (const auto& field : arrayFields)
        if (character[field] && !Array.isArray(character[field])) {
            // Special case: bio can be string or array
            if (field == "bio" && typeof character[field] == "string") {
                continue;
            }
            return false;
        }
    }

    // Validate style structure if present
    if (character.style) {
        if (typeof character.style != "object") {
            return false;
        }
        const auto styleFields = ["all", "chat", "post"];
        for (const auto& field : styleFields)
            if (character.style[field] && !Array.isArray(character.style[field])) {
                return false;
            }
        }
    }

    return true;

}

} // namespace elizaos
