#include "token-registration-listener-solana.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> startSolanaListener() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (isListening) {
        std::cout << "[Solana Listener] Already listening" << std::endl;
        return;
    }

    const auto programId = process.env.NEXT_PUBLIC_SOLANA_PROGRAM_ID;
    if (!programId) {
        std::cerr << "[Solana Listener] SOLANA_PROGRAM_ID not configured" << std::endl;
        return;
    }

    const auto rpcUrl =;
    process.env.NEXT_PUBLIC_SOLANA_RPC || "https://api.mainnet-beta.solana.com";
    connection = new Connection(rpcUrl, "confirmed");

    try {
        std::cout << "[Solana Listener] Starting listener for program" << programId << std::endl;
        isListening = true;

        // Subscribe to program logs
        const auto subscriptionId = connection.onLogs(;
        new PublicKey(programId),
        async (logs: Logs) => {
            handleProgramLogs(logs);
            },
            "confirmed",
            );

            // Handle graceful shutdown
            process.on("SIGINT", async () => {
                std::cout << "[Solana Listener] Stopping..." << std::endl;
                if (connection) {
                    connection.removeOnLogsListener(subscriptionId);
                }
                isListening = false;
                });

                process.on("SIGTERM", async () => {
                    std::cout << "[Solana Listener] Stopping..." << std::endl;
                    if (connection) {
                        connection.removeOnLogsListener(subscriptionId);
                    }
                    isListening = false;
                    });

                    std::cout << "[Solana Listener] Now listening for token registrations" << std::endl;
                    } catch (error) {
                        std::cerr << "[Solana Listener] Failed to start:" << error << std::endl;
                        isListening = false;
                    }

}

std::future<void> handleProgramLogs(Logs logs) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check if this is a register_token instruction
    const auto logMessages = logs.logs;

    // Look for register_token instruction signature
    const auto hasRegisterToken = logMessages.some(;
    (log: string) =>
    (std::find(log.begin(), log.end(), "Instruction: RegisterToken") != log.end()) ||
    (std::find(log.begin(), log.end(), "register_token") != log.end()),
    );

    if (!hasRegisterToken) {
        return;
    }

    std::cout << "[Solana Listener] Token registration detected:" << logs.signature << std::endl;

    // Parse transaction to extract token details
    if (connection) {
        const auto tx = connection.getTransaction(logs.signature, {;
            commitment: "confirmed",
            maxSupportedTransactionVersion: 0,
            });

            if (tx && tx.meta && tx.meta.logMessages) {
                parseRegisterTokenTransaction(tx);
            }
        }

}

std::future<void> parseRegisterTokenTransaction(VersionedTransactionResponse tx) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Extract token mint and metadata from transaction
    // This is simplified - actual implementation would parse instruction data

    // Get first signature from the transaction
    const auto signature = tx.transaction.signatures[0];

    // For now, log that we detected a registration
    console.log(
    "[Solana Listener] Detected token registration transaction:",
    signature,
    );

    // TODO: Parse instruction data to extract:
    // - token_mint address
    // - price_feed_id
    // - Register token to database

    console.warn(
    "[Solana Listener] Solana registration parsing not yet implemented",
    );

}

std::future<void> backfillSolanaEvents(std::optional<std::vector<std::string>> signatures) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto programId = process.env.NEXT_PUBLIC_SOLANA_PROGRAM_ID;
        if (!programId) {
            throw std::runtime_error("SOLANA_PROGRAM_ID not configured");
        }

        const auto rpcUrl =;
        process.env.NEXT_PUBLIC_SOLANA_RPC || "https://api.mainnet-beta.solana.com";
        const auto conn = new Connection(rpcUrl, "confirmed");

        console.log(
        "[Solana Backfill] Fetching recent transactions for program",
        programId,
        );

        // Get recent signatures
        const auto sigs =;
        signatures ||;
        (;
        conn.getSignaturesForAddress(new PublicKey(programId), {
            limit: 100,
            });
            ).map((s) => s.signature);

            std::cout << "[Solana Backfill] Found " + sigs.size() + " transactions" << std::endl;

            for (const auto& sig : sigs)
                try {
                    const auto tx = conn.getTransaction(sig, {;
                        commitment: "confirmed",
                        maxSupportedTransactionVersion: 0,
                        });

                        if (tx && tx.meta && tx.meta.logMessages) {
                            const auto hasRegisterToken = tx.meta.logMessages.some(;
                            (log) =>;
                            (std::find(log.begin(), log.end(), "Instruction: RegisterToken") != log.end()) ||
                            (std::find(log.begin(), log.end(), "register_token") != log.end()),
                            );

                            if (hasRegisterToken) {
                                parseRegisterTokenTransaction(tx);
                            }
                        }
                        } catch (error) {
                            console.warn(
                            "[Solana Backfill] Failed to process signature " + sig + ":"
                            error,
                            );
                        }
                    }

                    std::cout << "[Solana Backfill] ✅ Backfill complete" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
