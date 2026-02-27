#include "solana.hpp"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace elizaos {

void initSolanaConfig() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Set up network and RPC URL
        const auto network = std::getenv("NETWORK");
        const auto rpcUrl = getRpcUrl();

        // Create UMI instance
        const auto umi = createUmi(rpcUrl);

        // Set up program ID based on network
        const auto programId =;
        network == "devnet";
        ? std::getenv("DEVNET_PROGRAM_ID") || std::getenv("PROGRAM_ID");
        : std::getenv("PROGRAM_ID");

        if (!programId) {
            throw std::runtime_error("missing program_id env var");
        }

        // Create wallet if private key is available
        auto wallet: Keypair | std::nullopt;

        if(!std::getenv("EXECUTOR_PRIVATE_KEY")){
            throw std::runtime_error("missing EXECUTOR_PRIVATE_KEY env var");
        }

        try {
            wallet = Keypair.fromSecretKey(;
            Uint8Array.from(/* JSON::parse */ std::getenv("EXECUTOR_PRIVATE_KEY")),
            );
            std::cout << "Created wallet from std::getenv("EXECUTOR_PRIVATE_KEY")" << std::endl;
            } catch (error) {
                std::cerr << "Failed to create wallet from env:" << error << std::endl;
            }

            // Return configuration object
            return {
                umi,
                connection: new Connection(rpcUrl),
                program: nullptr, // Will be initialized later if anchor is used
                programId: new PublicKey(programId),
                wallet,
                network,
                };

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
