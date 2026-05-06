#include "models.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

AbstractModel getModel(ModelConfig config, ToolConfig tools) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    switch (config.name) {
        // case "human":
        return new HumanModel(config, tools);
        // case "human_thought":
        // HumanThoughtModel expects HumanModelConfig which has costPerCall
        const HumanModelConfig humanThoughtConfig = {;
            name: "human",
            costPerCall: 0,
            catchEof: (config).catchEof,
            };
            return new HumanThoughtModel(humanThoughtConfig, tools);
            // case "replay":
            return new ReplayModel(config, tools);
            // case "instant_empty_submit":
            return new InstantEmptySubmitModel(config, tools);
            // default:
            return new LiteLLMModel(config, tools);
        }

}

} // namespace elizaos
