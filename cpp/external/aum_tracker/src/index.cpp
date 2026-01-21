#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> startupPrefetch() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "🚀 Starting full prefetch on server startup..." << std::endl;

    try {
        // Create prefetch service with full startup configuration
        const auto prefetchService = new PrefetchService({;
            forceRefresh: true, // Always force refresh on startup for fresh data
            resumeFromFailures: true, // Resume from any previous failures
            // No limit - process all wallets on startup
            });

            // Run prefetch in background without blocking server start
            prefetchService.run().catch((error) => {
                std::cerr << "❌ Startup prefetch failed:" << error << std::endl;
                });
                } catch (error) {
                    std::cerr << "❌ Failed to start auto-prefetch:" << error << std::endl;
                }

}

} // namespace elizaos
