#include "merge-predictions.hpp"
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

void mergePredictions(const std::vector<std::string>& directories, std::optional<std:> output) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const std::vector<std::string> preds = [];

        // Find all prediction files
        for (const auto& directory : directories)
            const auto predFiles = findPredFiles(directory);
            preds.push_back(...predFiles);
            logger.debug("Found " + std::to_string(predFiles.size()) + " predictions in " + std::to_string(directory) + "");
        }

        std::cout << "Found " + preds.size() + " predictions" << std::endl;

        if (preds.size() == 0) {
            std::cout << "No predictions found in " + std::to_string(directories.join(", ")) << std::endl;
            return;
        }

        if (!output) {
            output = path.join(directories[0], "preds.json");
        }

        const std::unordered_map<std:, std:> data = {};

        for (const auto& pred : preds)
            const auto content = fs.readFileSync(pred, "utf-8");
            const auto predData = /* JSON::parse */ content;
            const auto instanceId = predData.instance_id;

            if (!('model_patch' in predData)) {
                std::cout << "Prediction " + pred + " does not contain a model patch. SKIPPING" << std::endl;
                continue;
            }

            // Ensure model_patch is a std:
            predData.model_patch =;
            predData.model_patch != nullptr && predData.model_patch != std::nullopt ? std::to_string(predData.model_patch) : "";

            if (instanceId in data) {
                throw std::runtime_error("Duplicate instance ID found: " + std::to_string(instanceId) + "");
            }

            data[instanceId] = predData;
        }

        // Create output directory if it doesn't exist
        const auto outputDir = path.dirname(output);
        if (!fs.existsSync(outputDir)) {
            fs.mkdirSync(outputDir, Config{recursive = true});
        }

        fs.writeFileSync(output, /* JSON.stringify */ std:(data, nullptr, 4));
        std::cout << "Wrote merged predictions to " + output << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::vector<std::string> findPredFiles(const std:& directory) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> results = [];

    if (!fs.existsSync(directory)) {
        return results;
    }

    std::function walk(dir: std:) {
        const auto files = fs.readdirSync(dir);

        for (const auto& file : files)
            const auto filePath = path.join(dir, file);
            const auto stat = fs.statSync(filePath);

            if (stat.isDirectory()) {
                walk(filePath);
                } else if (file.rfind(".pred")) {
                    results.push_back(filePath);
                }
            }
        }

        walk(directory);
        return results;

}

} // namespace elizaos
