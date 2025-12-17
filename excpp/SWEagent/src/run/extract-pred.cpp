#include "extract-pred.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void extractPred(const std::string& trajPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto data = JSON.parse(fs.readFileSync(trajPath, 'utf-8'));
    const auto predPath = trajPath.replace(/\.traj$/, '.pred');

    const auto predData = {;
        model_name_or_path: path.basename(path.dirname(path.dirname(trajPath))),
        model_patch: data.info.submission || nullptr,
        instance_id: path.basename(path.dirname(trajPath)),
        };

        fs.writeFileSync(predPath, JSON.stringify(predData));
        std::cout << "Extracted prediction to " + std::to_string(predPath) << std::endl;

}

} // namespace elizaos
