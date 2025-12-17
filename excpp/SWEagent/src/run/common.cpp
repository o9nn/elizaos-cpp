#include "common.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string shortenString(const std::string& s, double maxLength, bool shortenLeft = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (s.length <= maxLength) {
        return s;
    }

    if (shortenLeft) {
        return "..." + s.slice(s.size() - maxLength + 3);
        } else {
            return s.slice(0, maxLength - 3) + "...";
        }

}

std::any shortenStrings(const std::any& data, double maxLength = 30) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof data == 'string') {
        return shortenstd::to_string(data, maxLength);
    }

    if (Array.isArray(data)) {
        return data.map((item) => shortenStrings(item, maxLength));
    }

    if (data && typeof data == 'object') {
        const std::unordered_map<std::string, std::any> result = {};
        for (const int [key, value] of Object.entries(data)) {
            result[key] = shortenStrings(value, maxLength);
        }
        return result;
    }

    return data;

}

void savePredictions(const std::variant<std::string, path::ParsedPath>& trajDir, const std::string& instanceId, AgentRunResult result) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto dirPath = typeof trajDir == "string" ? trajDir : path.format(trajDir);
    const auto predPath = path.join(dirPath, "predictions.json");

    // Load existing predictions or create new
    std::unordered_map<std::string, std::any> predictions = {};
    if (fs.existsSync(predPath)) {
        const auto content = fs.readFileSync(predPath, "utf-8");
        predictions = /* JSON.parse */ content;
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
        fs.writeFileSync(predPath, /* JSON.stringify */ std::string(predictions, nullptr, 2));

}

bool isPromisingPatch(AgentInfo info) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto submission = info.submission;

    if (!submission || typeof submission != 'string') {
        return false;
    }

    // Check if patch is empty or only whitespace
    if (submission.trim() == '') {
        return false;
    }

    // Check if patch only contains diff headers but no actual changes
    const auto lines = submission.split("\n");
    auto hasChanges = false;

    for (const auto& line : lines)
        if (line.startsWith('+') && !line.startsWith('+++')) {
            hasChanges = true;
            break;
        }
        if (line.startsWith('-') && !line.startsWith('---')) {
            hasChanges = true;
            break;
        }
    }

    return hasChanges;

}

std::unordered_map<std::string, std::any> createNestedDict() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new Proxy(;
    {},
    {
        get: (target: Record<string, unknown>, prop: string) => {
            if (!(prop in target)) {
                target[prop] = createNestedDict();
            }
            return target[prop];
            },
            },
            );

}

std::unordered_map<std::string, std::any> parseArgsToNestedDict(const std::vector<std::string>& args) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto result = createNestedDict();

    for (const auto& arg : args)
        if (arg.includes('=')) {
            const auto [keyPath, value] = arg.split("=", 2);
            const auto keys = keyPath.split(".");

            std::unordered_map<std::string, std::any> current = result<string, unknown>;
            for (int i = 0; i < keys.length - 1; i++) {
                if (!(keys[i] in current)) {
                    current[keys[i]] = {}
                }
                current = current[keys[i]]<string, unknown>;
            }

            // Try to parse value as JSON, number, or boolean
            std::any parsedValue = value;
            try {
                parsedValue = /* JSON.parse */ value;
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
