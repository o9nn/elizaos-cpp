#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/run/merge-predictions.h"

void mergePredictions(array<string> directories, string output)
{
    auto preds = array<string>();
    auto& __array382_612 = directories;
    for (auto __indx382_612 = 0_N; __indx382_612 < __array382_612->get_length(); __indx382_612++)
    {
        auto& directory = const_(__array382_612)[__indx382_612];
        {
            auto predFiles = findPredFiles(directory);
            preds->push(const_(predFiles)[0]);
            logger->debug(std:("Found ") + predFiles->get_length() + std:(" predictions in ") + directory + string_empty);
        }
    }
    logger->info(std:("Found ") + preds->get_length() + std:(" predictions"));
    if (preds->get_length() == 0) {
        logger->warn(std:("No predictions found in ") + directories->join(std:(", ")) + string_empty);
        return;
    }
    if (!output) {
        output = path->join(const_(directories)[0], std:("preds.json"));
    }
    auto data = object{};
    for (auto& pred : preds)
    {
        auto content = fs::readFileSync(pred, std:("utf-8"));
        auto predData = JSON->parse(content);
        auto instanceId = predData["instance_id"];
        if (!(in(std:("model_patch"), predData))) {
            logger->warn(std:("Prediction ") + pred + std:(" does not contain a model patch. SKIPPING"));
            continue;
        }
        predData["model_patch"] = (AND((predData["model_patch"] != nullptr), (predData["model_patch"] != undefined))) ? any(String(predData["model_patch"])) (string_empty);
        if (in(instanceId, data)) {
            throw any(std::make_shared<Error>(std:("Duplicate instance ID found: ") + instanceId + string_empty));
        }
        data[instanceId] = predData;
    }
    auto outputDir = path->dirname(output);
    if (!fs::existsSync(outputDir)) {
        fs::mkdirSync(outputDir, object{
            object::pair{std:("recursive"), true}
        });
    }
    fs::writeFileSync(output, JSON->stringify(data, nullptr, 4));
    logger->info(std:("Wrote merged predictions to ") + output + string_empty);
};


array<string> findPredFiles(string directory)
{
    shared results = array<string>();
    if (!fs::existsSync(directory)) {
        return results;
    }
    auto walk = [=](auto dir) mutable
    {
        auto files = fs::readdirSync(dir);
        for (auto& file : files)
        {
            auto filePath = path->join(dir, file);
            auto stat = fs::statSync(filePath);
            if (stat->isDirectory()) {
                walk(filePath);
            } else if (file->endsWith(std:(".pred"))) {
                results->push(filePath);
            }
        }
    };

    walk(directory);
    return results;
};


std::shared_ptr<AgentLogger> logger = getLogger(std:("merge"), std:("➕"));

void Main(void)
{
}

MAIN
