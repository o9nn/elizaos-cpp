#include "merge-predictions.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void mergePredictions(const std::vector<std::string>& directories, std::optional<std::string> output) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const std::vector<std::string> preds = [];

        // Find all prediction files
        for (const auto& directory : directories)
            const auto predFiles = findPredFiles(directory);
            preds.push(...predFiles);
            logger.debug(`Found ${predFiles.length} predictions in ${directory}`);
        }

        std::cout << "Found " + std::to_string(preds.length) + " predictions" << std::endl;

        if (preds.length == 0) {
            std::cout << "No predictions found in " + std::to_string(directories.join(', ')) << std::endl;
            return;
        }

        if (!output) {
            output = path.join(directories[0], 'preds.json');
        }

        const std::unordered_map<std::string, std::any> data = {};

        for (const auto& pred : preds)
            const auto content = fs.readFileSync(pred, 'utf-8');
            const auto predData = JSON.parse(content);
            const auto instanceId = predData.instance_id;

            if (!('model_patch' in predData)) {
                std::cout << "Prediction " + std::to_string(pred) + " does not contain a model patch. SKIPPING" << std::endl;
                continue;
            }

            // Ensure model_patch is a string
            predData.model_patch =;
            predData.model_patch != nullptr && predData.model_patch != std::nullopt ? std::to_string(predData.model_patch) : '';

            if (instanceId in data) {
                throw std::runtime_error(`Duplicate instance ID found: ${instanceId}`);
            }

            data[instanceId] = predData;
        }

        // Create output directory if it doesn't exist
        const auto outputDir = path.dirname(output);
        if (!fs.existsSync(outputDir)) {
            fs.mkdirSync(outputDir, { recursive: true });
        }

        fs.writeFileSync(output, JSON.stringify(data, nullptr, 4));
        std::cout << "Wrote merged predictions to " + std::to_string(output) << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::vector<std::string> findPredFiles(const std::string& directory) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> results = [];

    if (!fs.existsSync(directory)) {
        return results;
    }

    function walk(dir: string) {
        const auto files = fs.readdirSync(dir);

        for (const auto& file : files)
            const auto filePath = path.join(dir, file);
            const auto stat = fs.statSync(filePath);

            if (stat.isDirectory()) {
                walk(filePath);
                } else if (file.endsWith('.pred')) {
                    results.push(filePath);
                }
            }
        }

        walk(directory);
        return results;

}

} // namespace elizaos
