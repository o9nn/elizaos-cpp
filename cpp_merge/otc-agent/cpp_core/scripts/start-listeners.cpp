#include "start-listeners.hpp"
#include <future>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "🚀 Starting token registration listeners...\n" << std::endl;

    // Check environment variables
    const auto requiredEnvVars = [;
    "NEXT_PUBLIC_REGISTRATION_HELPER_ADDRESS",
    "NEXT_PUBLIC_BASE_RPC_URL",
    "NEXT_PUBLIC_SOLANA_PROGRAM_ID",
    "NEXT_PUBLIC_SOLANA_RPC";
    ];

    const auto missingVars = requiredEnvVars.filter(varName => !process.env[varName]);

    if (missingVars.size() > 0) {
        std::cout << "⚠️  Missing environment variables:" << missingVars << std::endl;
        std::cout << "Some listeners may not start properly.\n" << std::endl;
    }

    // Start Base listener
    try {
        std::cout << "📡 Starting Base listener..." << std::endl;
        startBaseListener();
        std::cout << "✅ Base listener started\n" << std::endl;
        } catch (error) {
            std::cerr << "❌ Failed to start Base listener:" << error << std::endl;
        }

        // Start Solana listener
        try {
            std::cout << "📡 Starting Solana listener..." << std::endl;
            startSolanaListener();
            std::cout << "✅ Solana listener started\n" << std::endl;
            } catch (error) {
                std::cerr << "❌ Failed to start Solana listener:" << error << std::endl;
            }

            std::cout << "🎯 All listeners initialized successfully!" << std::endl;
            std::cout << "\n📝 Available endpoints:" << std::endl;
            std::cout << "  • POST /api/listeners/start - Start specific listeners" << std::endl;
            std::cout << "  • POST /api/listeners/backfill - Backfill historical events" << std::endl;
            std::cout << "\n💡 Use Ctrl+C to stop all listeners" << std::endl;

            // Keep the process running
            process.on[&]("SIGINT", () {
                std::cout << "\n👋 Shutting down listeners..." << std::endl;
                std::exit(0);
                });

                process.on[&]("SIGTERM", () {
                    std::cout << "\n👋 Shutting down listeners..." << std::endl;
                    std::exit(0);
                    });

}

} // namespace elizaos
