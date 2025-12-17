#include "environment.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<SolanaConfig> validateSolanaConfig(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto config = {;
                WALLET_SECRET_SALT:
                runtime.getSetting('WALLET_SECRET_SALT') || process.env.WALLET_SECRET_SALT,
                WALLET_SECRET_KEY: runtime.getSetting('WALLET_SECRET_KEY') || process.env.WALLET_SECRET_KEY,
                WALLET_PUBLIC_KEY:
                runtime.getSetting('SOLANA_PUBLIC_KEY') ||;
                runtime.getSetting('WALLET_PUBLIC_KEY') ||;
                process.env.WALLET_PUBLIC_KEY,
                SOL_ADDRESS: runtime.getSetting('SOL_ADDRESS') || process.env.SOL_ADDRESS,
                SLIPPAGE: runtime.getSetting('SLIPPAGE') || process.env.SLIPPAGE,
                SOLANA_RPC_URL: runtime.getSetting('SOLANA_RPC_URL') || process.env.SOLANA_RPC_URL,
                HELIUS_API_KEY: runtime.getSetting('HELIUS_API_KEY') || process.env.HELIUS_API_KEY,
                BIRDEYE_API_KEY: runtime.getSetting('BIRDEYE_API_KEY') || process.env.BIRDEYE_API_KEY,
                };

                return solanaEnvSchema.parse(config);
                } catch (error) {
                    if (error instanceof z.ZodError) {
                        const auto errorMessages = error.errors;
                        std::to_string(err.path.join('.')) + ": " + std::to_string(err.message)
                        .join('\n');
                        throw std::runtime_error(`Solana configuration validation failed:\n${errorMessages}`);
                    }
                    throw;
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
