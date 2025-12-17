#include "claimFees.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> claimFees(PublicKey nftMint, PublicKey poolId, Connection connection, PublicKey claimer, WebSocketClient websocket, Token token) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto wallet = Keypair.fromSecretKey(;
            Uint8Array.from(/* JSON.parse */ process.env.EXECUTOR_PRIVATE_KEY!);
            );

            // Build an Anchor provider.
            const auto provider = new AnchorProvider(;
            connection,
            new Wallet(wallet),
            AnchorProvider.defaultOptions();
            );

            const auto program = new Program<RaydiumVault>(;
            raydium_vault_IDL,
            provider;
            );
            // try this 3 times if it fails
            // const maxRetries = 3;
            // let attempt = 0;
            // let success = false;
            auto txSignature = "";
            // while (attempt < maxRetries && !success) {
            try {
                txSignature = claim(;
                program,
                wallet,
                nftMint,
                poolId,
                connection,
                claimer,
                token;
                );
                } catch (error) {
                    std::cerr << "Error during claim attempt:" << error << std::endl;
                    // attempt++;
                }
                // }
                if (!txSignature) {
                    throw std::runtime_error("Failed to claim after multiple attempts.");
                }
                "websocket.to(" + "claimer:" + std::to_string(claimer.toBase58())
                    txSignature,
                    poolId: poolId.toBase58(),
                    claimer: claimer.toBase58(),
                    });
                    std::cout << "Transaction Signature:" << txSignature << std::endl;
                    return txSignature;
                    } catch (error) {
                        std::cerr << "Error during claim:" << error << std::endl;
                        throw;
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
