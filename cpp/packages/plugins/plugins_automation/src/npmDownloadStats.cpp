#include "npmDownloadStats.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto generator = new NpmDownloadStatsGenerator();
    generator.main();

}

} // namespace elizaos
