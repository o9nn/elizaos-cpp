#include "run-batch.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> runBatchFromConfig(RunBatchConfig config) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto runner = RunBatch.fromConfig(config);
    runner.main();

}

} // namespace elizaos
