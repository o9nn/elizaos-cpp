#include "run-traj-to-demo.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void saveDemo(const std::any& data, const std::string& file, const std::string& trajPath) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto content = yaml.dump(data, {;
        lineWidth: -1,
        noRefs: true,
        quotingType: """,
        forceQuotes: false,
        });

        const auto header = DEMO_COMMENT.replace("{traj_path}", trajPath);
        "fs.writeFileSync(file, " + header + "\n" + content;

}

void convertTrajToActionDemo(const std::string& trajPath, const std::string& outputFile, bool includeUser = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto traj = /* JSON.parse */ fs.readFileSync(trajPath, "utf-8");

    auto replayConfig = traj.replay_config;
    if (typeof traj.replay_config == 'string') {
        replayConfig = /* JSON.parse */ traj.replay_config;
    }

    const auto history = traj.history || [];

    const auto copyFields = new Set(["content", "role", "tool_calls", "agent", "message_type", "tool_call_ids"]);

    const auto admissibleRoles = includeUser ? new Set(["assistant", "user", "tool"]) : new Set(["assistant"]);

    const auto filteredHistory = history;
    .filter(;
    (step: Record<string, unknown>) =>
    admissibleRoles.has(step.role) &&;
    (!step.agent || step.agent == "main" || step.agent == "primary") &&;
    !step.is_demo,
    );
    .map((step: Record<string, unknown>) => {
        const std::unordered_map<std::string, std::any> filtered = {};
        for (const auto& key : copyFields)
            if (key in step) {
                filtered[key] = step[key];
            }
        }
        return filtered;
        });

        const auto outputData = {;
            history: filteredHistory,
            replay_config: replayConfig,
            };

            saveDemo(outputData, outputFile, trajPath);
            std::cout << "Saved demo to " + outputFile << std::endl;

}

void trajToDemo(const std::string& trajPath, std::string outputDir = "./demos", std::string suffix = "", bool overwrite = false, bool includeUser = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto trajDir = path.dirname(trajPath);
        const auto trajName = path.basename(trajPath, ".traj");
        const auto outputSubdir = path.basename(trajDir) + suffix;
        const auto outputFile = "path.join(outputDir, outputSubdir, " + trajName + ".demo.yaml";

        if (fs.existsSync(outputFile) && !overwrite) {
            throw std::runtime_error(`Output file already exists: ${outputFile}. Use --overwrite to overwrite.`);
        }

        const auto outputFileDir = path.dirname(outputFile);
        if (!fs.existsSync(outputFileDir)) {
            fs.mkdirSync(outputFileDir, { recursive: true });
        }

        convertTrajToActionDemo(trajPath, outputFile, includeUser);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
