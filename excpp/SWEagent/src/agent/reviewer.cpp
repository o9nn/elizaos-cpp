#include "reviewer.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::variant<ScoreRetryLoop, ChooserRetryLoop> getRetryLoopFromConfig(RetryLoopConfig config, ProblemStatement problemStatement) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        switch (config.type) {
            case 'score':
            return new ScoreRetryLoop(config, problemStatement);
            case 'chooser':
            return new ChooserRetryLoop(config, problemStatement);
            default:
            throw std::runtime_error(`Unknown retry loop type: ${(config as RetryLoopConfig).type}`);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
