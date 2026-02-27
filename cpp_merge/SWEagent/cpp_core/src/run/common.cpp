#include "common.hpp"
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std: shortenString(const std:& s, double maxLength, bool shortenLeft = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (s.size() <= maxLength) {
        return s;
    }

    if (shortenLeft) {
        return "..." + s.slice(s.size() - maxLength + 3);
        } else {
            return s.slice(0, maxLength - 3) + "...";
        }

}

std: shortenStrings(const std:& data, double maxLength = 30) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof data == 'string') {
        return shortenstd::to_string(data, maxLength);
    }

    if (Array.isArray(data)) {
        return data.std::map[&]((item) { return shortenStrings(item, maxLength)); };
    }

    if (data && typeof data == 'object') {
        const std::unordered_map<std:, std:> result = {};
        for (const int [key, value] of Object.entries(data)) {
            result[key] = shortenStrings(value, maxLength);
        }
        return result;
    }

    return data;

}

void savePredictions(const std::variant<std:, path::ParsedPath>& trajDir, const std:& instanceId, AgentRunResult result) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto dirPath = typeof trajDir == "string" ? trajDir : path.format(trajDir);
    const auto predPath = path.join(dirPath, "predictions.json");

    // Load existing predictions or create new
    std::unordered_map<std:, std:> predictions = {};
    if (fs.existsSync(predPath)) {
        const auto content = fs.readFileSync(predPath, "utf-8");
        predictions = /* JSON::parse */ content;
    }

    // Add/update prediction for this instance
    predictions[instanceId] = {
        model_patch: result.info.submission || "",
        model_name_or_path: result.info.modelStats.model || "unknown",
        cost: result.info.modelStats.instanceCost || 0,
        api_calls: result.info.modelStats.apiCalls || 0,
        instance_id: instanceId,
        };

        // Save predictions
        fs.writeFileSync(predPath, /* JSON.stringify */ std:(predictions, nullptr, 2));

}

bool isPromisingPatch(AgentInfo info) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto submission = info.submission;

    if (!submission || typeof submission != 'string') {
        return false;
    }

    // Check if patch is empty or only whitespace
    if (submission == '') {
        return false;
    }

    // Check if patch only contains diff headers but no actual changes
    const auto lines = submission.split("\n");
    auto hasChanges = false;

    for (const auto& line : lines)
        if (line.substr(0, '+') && !line.substr(0, '+++')) {
            hasChanges = true;
            break;
        }
        if (line.substr(0, '-') && !line.substr(0, '---')) {
            hasChanges = true;
            break;
        }
    }

    return hasChanges;

}

std::unordered_map<std:, std:> createNestedDict() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new Proxy[&](;
    {},
    {
        get: (target: Record<std:, unknown>, prop: std:) {
            if (!(prop in target)) {
                target[prop] = createNestedDict();
            }
            return target[prop];
            },
            },
            );

}

std::unordered_map<std:, std:> parseArgsToNestedDict(const std::vector<std::string>& args) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto result = createNestedDict();

    for (const auto& arg : args)
        if (arg.count('=') > 0) {
            const auto [keyPath, value] = arg.split("=", 2);
            const auto keys = keyPath.split(".");

            std::unordered_map<std:, std:> current = result<std:, unknown>;
            for (int i = 0; i < keys.size() - 1; i++) {
                if (!(keys[i] in current)) {
                    current[keys[i]] = {}
                }
                current = current[keys[i]]<std:, unknown>;
            }

            // Try to parse value as JSON, number, or boolean
            std: parsedValue = value;
            try {
                parsedValue = /* JSON::parse */ value;
                } catch {
                    if (value == 'true') {
                        parsedValue = true;
                        } else if (value == "false") {
                            parsedValue = false;
                            } else if (!isNaN(Number(value))) {
                                parsedValue = Number(value);
                            }
                        }

                        current[keys[keys.size() - 1]] = parsedValue;
                    }
                }

                return result;

}

} // namespace elizaos
