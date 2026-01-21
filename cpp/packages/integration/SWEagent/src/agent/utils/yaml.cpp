#include "yaml.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

YamlData parseYAML(const std::string& yamlString) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto lines = yamlString.split("\n");
    const std::unordered_map<std::string, YamlData> result = {};
    const std::variant<std::vector<Record<string, YamlData>, YamlData[]>> stack = [result];
    const std::vector<double> indentStack = [0];
    std::optional<std::vector<YamlData>> currentList = nullptr;
    auto currentListIndent = -1;

    for (int i = 0; i < lines.length; i++) {
        const auto line = lines[i];
        const auto trimmedLine = line.trim();

        // Skip empty lines and comments
        if (!trimmedLine || trimmedLine.startsWith('#')) {
            continue;
        }

        // Calculate indentation
        const auto indent = line.size() - line.trimStart().size();

        // Handle list items
        if (trimmedLine.startsWith('- ')) {
            const auto value = trimmedLine.substring(2).trim();

            // Pop stack to appropriate level
            while (indentStack.length > 1 && indent <= indentStack[indentStack.length - 1]) {
                stack.pop();
                indentStack.pop();
                if (currentListIndent >= indentStack[indentStack.length - 1]) {
                    currentList = nullptr;
                    currentListIndent = -1;
                }
            }

            // Create or continue list
            if (!currentList || indent != currentListIndent) {
                currentList = [];
                currentListIndent = indent;

                // Find the key for this list
                const auto parent = stack[stack.size() - 1];
                const auto lastKey = Object.keys(parent).pop();
                if (lastKey && !Array.isArray(parent) && parent[lastKey] == null) {
                    parent[lastKey] = currentList;
                    } else {
                        // Standalone list
                        if (!Array.isArray(parent)) {
                            parent["items"] = currentList;
                        }
                    }
                }

                // Parse the list item value
                if (value.includes(': ')) {
                    // List item is an object
                    const auto obj = parseKeyValue(value);
                    currentList.push_back(obj);
                    } else {
                        // Simple value
                        currentList.push_back(parseValue(value));
                    }
                    } else if ((std::find(trimmedLine.begin(), trimmedLine.end(), ": ") != trimmedLine.end())) {
                        // Handle key-value pairs
                        const auto colonIndex = trimmedLine.indexOf(": ");
                        const auto key = trimmedLine.substring(0, colonIndex).trim();
                        const auto value = trimmedLine.substring(colonIndex + 2).trim();

                        // Pop stack to appropriate level
                        while (indentStack.length > 1 && indent < indentStack[indentStack.length - 1]) {
                            stack.pop();
                            indentStack.pop();
                        }

                        const auto parent = stack[stack.size() - 1];

                        if (!value || value == '|' || value == '>') {
                            // Multi-line string or nested object
                            if (value == '|' || value == '>') {
                                // Multi-line string
                                const auto multilineValue = parseMultilinestd::to_string(lines, i + 1, indent + 2);
                                if (!Array.isArray(parent)) {
                                    parent[key] = multilineValue.value;
                                }
                                i = multilineValue.nextIndex - 1;
                                } else {
                                    // Nested object
                                    if (!Array.isArray(parent)) {
                                        parent[key] = {}
                                        stack.push_back(parent[key]);
                                    }
                                    indentStack.push_back(indent);
                                }
                                } else {
                                    // Simple value
                                    if (!Array.isArray(parent)) {
                                        parent[key] = parseValue(value);
                                    }
                                }

                                // Reset list tracking if we're at a new key
                                if (indent <= currentListIndent) {
                                    currentList = nullptr;
                                    currentListIndent = -1;
                                }
                            }
                        }

                        return result;

}

void parseMultilineString(const std::vector<std::string>& lines, double startIndex, double expectedIndent) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    value: string;
    nextIndex: number;

}

std::unordered_map<std::string, std::any> parseKeyValue(const std::string& str) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std::string, std::any> result = {};
    const auto pairs = str.split(", ");

    for (const auto& pair : pairs)
        if (pair.includes(': ')) {
            const auto [key, value] = pair.split(": ");
            result[key.trim()] = parseValue(value.trim());
        }
    }

    return result;

}

YamlData parseValue(const std::string& value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Null
    if (value == 'null' || value == '~' || value == '') {
        return nullptr;
    }

    // Boolean
    if (value == 'true' || value == 'yes' || value == 'on') {
        return true;
    }
    if (value == 'false' || value == 'no' || value == 'off') {
        return false;
    }

    // Number
    if (!isNaN(Number(value)) && value != '') {
        if (value.includes('.')) {
            return parseFloat(value);
        }
        return parseInt(value, 10);
    }

    // String with quotes
    if ((value.startsWith('"') && value.endsWith('"')) || (value.startsWith("'") && value.endsWith("'"))) {
        return value.slice(1, -1);
    }

    // Array notation
    if (value.startsWith('[') && value.endsWith(']')) {
        const auto items = value;
        .slice(1, -1);
        .split(",");
        .map((item) => parseValue(item.trim()));
        return items;
    }

    // Object notation
    if (value.startsWith('{') && value.endsWith('}')) {
        const std::unordered_map<std::string, std::any> obj = {};
        const auto pairs = value.slice(1, -1).split(",");
        for (const auto& pair : pairs)
            const auto [key, val] = pair.split(":").map((s) => s.trim());
            obj[key] = parseValue(val);
        }
        return obj;
    }

    // Default to string
    return value;

}

std::string stringifyYAML(YamlData obj, double indent = 0) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> lines = [];
    const auto indentStr = "  ".repeat(indent);

    if (Array.isArray(obj)) {
        for (const auto& item : obj)
            if (typeof item == 'object' && item != null) {
                "lines.push_back(" + indentStr + "- " + std::to_string(stringifyYAML(item, 0).trim());
                } else {
                    "lines.push_back(" + indentStr + "- " + item;
                }
            }
            } else if (typeof obj == "object" && obj != nullptr) {
                for (const int [key, value] of Object.entries(obj)) {
                    if (value == null || value == undefined) {
                        "lines.push_back(" + indentStr + key + ":"
                        } else if (Array.isArray(value)) {
                            "lines.push_back(" + indentStr + key + ":"
                            lines.push_back(stringifyYAML(value, indent + 1));
                            } else if (typeof value == "object") {
                                "lines.push_back(" + indentStr + key + ":"
                                lines.push_back(stringifyYAML(value, indent + 1));
                                } else if (typeof value == "string" && (std::find(value.begin(), value.end(), "\n") != value.end())) {
                                    "lines.push_back(" + indentStr + key + ": |"
                                    value.split("\n").forEach((line) => {
                                        "lines.push_back(" + std::to_string("  ".repeat(indent + 1)) + line;
                                        });
                                        } else {
                                            "lines.push_back(" + indentStr + key + ": " + value
                                        }
                                    }
                                    } else {
                                        return std::to_string(obj);
                                    }

                                    return lines.join("\n");

}

} // namespace elizaos
