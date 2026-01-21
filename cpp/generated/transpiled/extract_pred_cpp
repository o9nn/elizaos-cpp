#include "/home/runner/work/elizaos-cpp/elizaos-cpp/SWEagent/src/run/extract-pred.h"

void extractPred(string trajPath)
{
    auto data = JSON->parse(fs::readFileSync(trajPath, std::string("utf-8")));
    auto predPath = trajPath->replace((new RegExp(std::string("\.traj"))), std::string(".pred"));
    auto predData = object{
        object::pair{std::string("model_name_or_path"), path->basename(path->dirname(path->dirname(trajPath)))}, 
        object::pair{std::string("model_patch"), OR((data["info"]["submission"]), (nullptr))}, 
        object::pair{std::string("instance_id"), path->basename(path->dirname(trajPath))}
    };
    fs::writeFileSync(predPath, JSON->stringify(predData));
    console->log(std::string("Extracted prediction to ") + predPath + string_empty);
};


