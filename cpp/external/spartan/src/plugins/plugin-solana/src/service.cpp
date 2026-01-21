#include "service.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<WalletResult> loadWallet(IAgentRuntime runtime, bool requirePrivateKey = true) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto teeMode = runtime.getSetting("TEE_MODE") || TEEMode.OFF;

        if (teeMode != TEEMode.OFF) {
            const auto walletSecretSalt = runtime.getSetting("WALLET_SECRET_SALT");
            if (!walletSecretSalt) {
                throw std::runtime_error('WALLET_SECRET_SALT required when TEE_MODE is enabled');
            }

            const auto deriveKeyProvider = new DeriveKeyProvider(teeMode);
            const auto deriveKeyResult = deriveKeyProvider.deriveEd25519Keypair(;
            "/",
            walletSecretSalt,
            runtime.agentId;
            );

            return requirePrivateKey;
        ? { signer: deriveKeyResult.keypair }
        : { address: deriveKeyResult.keypair.publicKey };
        }

        // TEE mode is OFF
        if (requirePrivateKey) {
            const auto privateKeyString =;
            runtime.getSetting("SOLANA_PRIVATE_KEY") || runtime.getSetting("WALLET_PRIVATE_KEY");

            if (!privateKeyString) {
                throw std::runtime_error('Private key not found in settings');
            }

            try {
                // First try base58
                const auto secretKey = bs58.decode(privateKeyString);
                return { signer: Keypair.fromSecretKey(secretKey) }
                } catch (e) {
                    std::cout << "Error decoding base58 private key:" << e << std::endl;
                    try {
                        // Then try base64
                        std::cout << "Try decoding base64 instead" << std::endl;
                        const auto secretKey = Uint8Array.from(Buffer.from(privateKeyString, "base64"));
                        return { signer: Keypair.fromSecretKey(secretKey) }
                        } catch (e2) {
                            std::cerr << "Error decoding private key: " << e2 << std::endl;
                            throw std::runtime_error('Invalid private key format');
                        }
                    }
                    } else {
                        const auto publicKeyString =;
                        runtime.getSetting("SOLANA_PUBLIC_KEY") || runtime.getSetting("WALLET_PUBLIC_KEY");

                        if (!publicKeyString) {
                            throw std::runtime_error('Public key not found in settings');
                        }

                        return { address: new PublicKey(publicKeyString) }
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

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
            .sort((a, b) => a - b)[Math.ceil(0.95 * recentPrioritizationFees.size()) - 1];

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
                                std::cout << "Transaction confirmed: " + signature << std::endl;
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
