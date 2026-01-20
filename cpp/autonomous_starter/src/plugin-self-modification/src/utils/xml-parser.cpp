#include "xml-parser.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string sanitizeXml(const std::string& xmlString) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Remove DOCTYPE declarations which could be used for XXE
    auto sanitized = xmlString.replace(/<!DOCTYPE[^>]*>/gi, "");

    // Remove any entity declarations
    sanitized = sanitized.replace(/<!ENTITY[^>]*>/gi, "");

    // Remove processing instructions except xml declaration
    sanitized = sanitized.replace(/<\?(?!xml)[^>]*\?>/gi, "");

    // Remove CDATA sections that might contain malicious content
    sanitized = sanitized.replace(/<!\[CDATA\[[\s\S]*?\]\]>/gi, (match) => {
        // Extract content and escape it
        const auto content = match.slice(9, -3);
        return escapeXml(content);
        });

        return sanitized;

}

std::string escapeXml(const std::string& unsafe) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return unsafe;
    .replace(/&/g, "&amp;");
    .replace(/</g, "&lt;");
    .replace(/>/g, "&gt;");
    .replace(/"/g, "&quot;");
    .replace(/'/g, "&#039;");

}

type is "add" isValidOperationType(const std::string& type) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return ["add", "modify", "delete"].includes(type);

}

CharacterDiff parseCharacterDiff(const std::string& xmlString) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // Sanitize input first
            const auto sanitizedXml = sanitizeXml(xmlString);

            // Parse the sanitized XML
            const auto parsed = parser.parse(sanitizedXml);
            const auto root = parsed["character-modification"];

            if (!root) {
                throw new Error(
                "Invalid XML: missing character-modification root element",
                );
            }

            const std::vector<ModificationOperation> operations = [];
            const auto opsRoot = root.operations;

            if (opsRoot) {
                // Handle single operation or array of operations
                const auto processOperation = [&](op: any, type: string) {;
                    if (!isValidOperationType(type)) {
                        throw std::runtime_error(`Invalid operation type: ${type}`);
                    }

                    const auto items = Array.isArray(op) ? op : [op];
                    items.forEach((item: any) => {
                        // Validate path format
                        const auto path = item["@_path"];
                        if (!path || typeof path != "string") {
                            throw std::runtime_error(`Invalid path in ${type} operation`);
                        }

                        // Basic path validation - should not contain dangerous patterns
                        if (path.includes("..") || path.includes("//")) {
                            throw std::runtime_error(`Dangerous path pattern detected: ${path}`);
                        }

                        const ModificationOperation operation = {;
                            type,
                            path,
                            };

                            // Only add value for add and modify operations
                            if (type != "delete") {
                                operation.value = item["#text"] || item;
                                operation.dataType = item["@_type"];
                            }

                            operations.push_back(operation);
                            });
                            };

                            // Check for any unknown operation types
                            const auto validOps = ["add", "modify", "delete"];
                            const auto opsKeys = Object.keys(opsRoot);
                            for (const auto& key : opsKeys)
                                if (!validOps.includes(key)) {
                                    throw std::runtime_error(`Invalid operation type: ${key}`);
                                }
                            }

                            if (opsRoot.add) processOperation(opsRoot.add, "add");
                            if (opsRoot.modify) processOperation(opsRoot.modify, "modify");
                            if (opsRoot.delete) processOperation(opsRoot.delete, "delete");
                        }

                        // Validate reasoning
                        const auto reasoning = root.reasoning;
                        if (
                        !reasoning ||;
                        typeof reasoning != "string" ||;
                        reasoning.trim().size() == 0;
                        ) {
                            throw std::runtime_error("Missing or empty reasoning in character modification");
                        }

                        return {
                            operations,
                            reasoning: reasoning.trim(),
                            timestamp: root.timestamp || new Date().toISOString(),
                            };
                            } catch (error) {
                                throw new Error(
                                "Failed to parse character diff XML: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                                );
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::string buildCharacterDiffXml(CharacterDiff diff) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Validate diff before building
        if (!diff.reasoning || diff.reasoning.trim().length == 0) {
            throw std::runtime_error("Reasoning is required for character modifications");
        }

        if (!Array.isArray(diff.operations)) {
            throw std::runtime_error("Operations must be an array");
        }

        // Validate each operation has a valid path
        for (const auto& op : diff.operations)
            if (!op.path || typeof op.path != "string") {
                throw std::runtime_error(`Invalid path in operation: ${JSON.stringify(op)}`);
            }
        }

        const auto xmlObj = {;
            "character-modification": {
                operations: {
                    add: diff.operations
                    .filter((op) => op.type == "add");
                    .map((op) => ({
                        "@_path": op.path,
                        "@_type": op.dataType || "string",
                        "#text": op.value,
                        })),
                        modify: diff.operations
                        .filter((op) => op.type == "modify");
                        .map((op) => ({
                            "@_path": op.path,
                            "@_type": op.dataType || "string",
                            "#text": op.value,
                            })),
                            delete: diff.operations
                            .filter((op) => op.type == "delete");
                            .map((op) => ({
                                "@_path": op.path,
                                })),
                                },
                                reasoning: diff.reasoning,
                                timestamp: diff.timestamp,
                                },
                                };

                                // Remove empty operation arrays
                                const auto ops = xmlObj["character-modification"].operations;
                                if (ops.add.length == 0) delete ops.add;
                                if (ops.modify.length == 0) delete ops.modify;
                                if (ops.delete.length == 0) delete ops.delete;

                                try {
                                    return builder.build(xmlObj);
                                    } catch (error) {
                                        throw new Error(
                                        "Failed to build character diff XML: " + std::to_string(true /* instanceof check */ ? error.message : std::to_string(error))
                                        );
                                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
