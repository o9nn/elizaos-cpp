#include "sendTransaction.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> sendTransaction(Connection connection, const std::vector<std::any>& instructions, Keypair wallet) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto latestBlockhash = connection.getLatestBlockhash();

        // Create a new TransactionMessage with the instructions
        const auto messageV0 = new TransactionMessage({;
            payerKey: wallet.publicKey,
            recentBlockhash: latestBlockhash.blockhash,
            instructions,
            }).compileToV0Message();

            // Estimate compute units
            const auto simulatedTx = new VersionedTransaction(messageV0);
            simulatedTx.sign([wallet]);
            const auto simulation = connection.simulateTransaction(simulatedTx);
            const auto computeUnits = simulation.value.unitsConsumed || 200_000;
            const auto safeComputeUnits = Math.ceil(Math.max(computeUnits * 1.3, computeUnits + 100_000));

            // Get prioritization fee
            const auto recentPrioritizationFees = connection.getRecentPrioritizationFees();
            const auto prioritizationFee = recentPrioritizationFees;
            .map((fee) => fee.prioritizationFee);
            .sort((a, b) => a - b)[Math.ceil(0.95 * recentPrioritizationFees.length) - 1];

            // Add compute budget instructions
            const auto computeBudgetInstructions = [;
            ComputeBudgetProgram.setComputeUnitLimit({ units: safeComputeUnits }),
            ComputeBudgetProgram.setComputeUnitPrice({ microLamports: prioritizationFee }),
            ];

            // Create final transaction
            const auto finalMessage = new TransactionMessage({;
                payerKey: wallet.publicKey,
                recentBlockhash: latestBlockhash.blockhash,
                instructions: [...computeBudgetInstructions, ...instructions],
                }).compileToV0Message();

                const auto transaction = new VersionedTransaction(finalMessage);
                transaction.sign([wallet]);

                // Send and confirm transaction
                const auto timeoutMs = 90000;
                const auto startTime = Date.now();

                while (Date.now() - startTime < timeoutMs) {
                    const auto transactionStartTime = Date.now();

                    const auto signature = connection.sendTransaction(transaction, {;
                        maxRetries: 0,
                        skipPreflight: true,
                        });

                        const auto statuses = connection.getSignatureStatuses([signature]);
                        if (statuses.value[0]) {
                            if (!statuses.value[0].err) {
                                "Transaction confirmed: " + std::to_string(signature)
                                return signature;
                                } else {
                                    throw std::runtime_error(`Transaction failed: ${statuses.value[0].err.toString()}`);
                                }
                            }

                            const auto elapsedTime = Date.now() - transactionStartTime;
                            const auto remainingTime = Math.max(0, 1000 - elapsedTime);
                            if (remainingTime > 0) {
                                new Promise((resolve) => setTimeout(resolve, remainingTime));
                            }
                        }

                        throw std::runtime_error('Transaction timeout');

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
