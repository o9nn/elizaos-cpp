#include "run-replay.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> runReplayFromConfig(RunReplayConfig config) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto runner = RunReplay.fromConfig(config);
    runner.main();

}

} // namespace elizaos
