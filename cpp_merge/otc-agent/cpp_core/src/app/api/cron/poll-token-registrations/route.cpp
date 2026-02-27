#include "route.hpp"
#include <string>
#include <future>
#include <cstdlib>
#include <optional>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<bigint> getLastBaseBlock() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envBlock = std::getenv("LAST_PROCESSED_BASE_BLOCK");
    if (envBlock) {
        return BigInt(envBlock);
    }
    return lastBaseBlock;

}

std::future<void> pollBaseRegistrations() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto registrationHelperAddress =;
    std::getenv("NEXT_PUBLIC_REGISTRATION_HELPER_ADDRESS");
    if (!registrationHelperAddress) {
        std::cerr << "[Cron] REGISTRATION_HELPER_ADDRESS not configured" << std::endl;
        return {
            processed: 0,
            error: "REGISTRATION_HELPER_ADDRESS not configured",
            };
        }

        const auto rpcUrl =;
        std::getenv("NEXT_PUBLIC_BASE_RPC_URL") || "https://mainnet.base.org";
        const auto client = createPublicClient({;
            chain: base,
            transport: http(rpcUrl),
            });

            try {
                const auto latestBlock = client.getBlockNumber();

                // If we don't have a last block, start from 1000 blocks ago (to catch up)
                // This ensures we don't miss events even if state resets
                const auto savedBlock = getLastBaseBlock();
                const auto startBlock = savedBlock || latestBlock - BigInt(1000);

                // Don't process if we're already up to date
                if (startBlock >= latestBlock) {
                    return { processed: 0, message: "Already up to date" }
                }

                console.log(
                "[Cron Base] Fetching events from block " + startBlock + " to " + latestBlock
                );

                const auto logs = client.getLogs({;
                    "address: registrationHelperAddress as " + "0x" + std::string event: {
                        type: "event",
                        name: "TokenRegistered",
                        inputs: [
                        { type: "bytes32", name: "tokenId", indexed: true },
                        { type: "address", name: "tokenAddress", indexed: true },
                        { type: "address", name: "pool", indexed: true },
                        { type: "address", name: "oracle" },
                        { type: "address", name: "registeredBy" },
                        ],
                        },
                        fromBlock: startBlock,
                        toBlock: latestBlock,
                        });

                        std::cout << "[Cron Base] Found " + logs.size() + " TokenRegistered events" << std::endl;

                        auto processed = 0;
                        for (const auto& log : logs)
                            const auto { tokenAddress, registeredBy } = log.args as {;
                                tokenId: std::string;
                                tokenAddress: std::string;
                                pool: std::string;
                                registeredBy: std::string;
                                };

                                console.log(
                                "[Cron Base] Processing token registration: " + tokenAddress + " by " + registeredBy
                                );

                                // Fetch token metadata
                                // Use type assertion to bypass viem's strict authorizationList requirement
                                const auto readContract = client.readContract as [&](;
                                params: unknown,
                                ) { return Promise<unknown>; };
                                const auto [symbol, name, decimals] = Promise.all([;
                                readContract({
                                    "address: tokenAddress as " + "0x" + std::string abi: ERC20_ABI,
                                    functionName: "symbol",
                                    }),
                                    readContract({
                                        "address: tokenAddress as " + "0x" + std::string abi: ERC20_ABI,
                                        functionName: "name",
                                        }),
                                        readContract({
                                            "address: tokenAddress as " + "0x" + std::string abi: ERC20_ABI,
                                            functionName: "decimals",
                                            }),
                                            ]);

                                            // Register to database
                                            const auto tokenService = std::make_unique<TokenRegistryService>();
                                            tokenService.registerToken({
                                                symbol: symbol,
                                                name: name,
                                                contractAddress: tokenAddress.toLowerCase(),
                                                chain: "base",
                                                decimals: Number(decimals),
                                                logoUrl: std::nullopt,
                                                "description: " + "Registered via RegistrationHelper by " + registeredBy
                                                });

                                                processed++;
                                                std::cout << "[Cron Base] ✅ Registered " + symbol + " (" + tokenAddress + ")" << std::endl;
                                            }

                                            // Update last processed block
                                            lastBaseBlock = latestBlock;

                                            return { processed, latestBlock: latestBlock.toString() }
                                            } catch (error) {
                                                std::cerr << "[Cron Base] Error:" << error << std::endl;
                                                return {
                                                    processed: 0,
                                                    error: true /* instanceof check */ ? error.message : "Unknown error",
                                                    };
                                                }

}

std::future<void> pollSolanaRegistrations() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto programId = std::getenv("NEXT_PUBLIC_SOLANA_PROGRAM_ID");
    if (!programId) {
        std::cerr << "[Cron] SOLANA_PROGRAM_ID not configured" << std::endl;
        return { processed: 0, error: "SOLANA_PROGRAM_ID not configured" }
    }

    const auto rpcUrl =;
    std::getenv("NEXT_PUBLIC_SOLANA_RPC") || "https://api.mainnet-beta.solana.com";
    const auto connection = new Connection(rpcUrl, "confirmed");

    try {
        // Get recent signatures for the program
        const auto signatures = connection.getSignaturesForAddress(;
        new PublicKey(programId),
        { limit: 50 }, // Check last 50 transactions
        );

        if (signatures.size() == 0) {
            return { processed: 0, message: "No recent transactions" }
        }

        // If we have a last signature, only process newer ones
        auto startIndex = 0;
        if (lastSolanaSignature) {
            const auto lastIndex = signatures.findIndex(;
            [&](sig) { return sig.signature == lastSolanaSignature,; }
            );
            if (lastIndex >= 0) {
                startIndex = lastIndex + 1;
            }
        }

        if (startIndex >= signatures.size()) {
            return { processed: 0, message: "Already up to date" }
        }

        console.log(
        "[Cron Solana] Checking " + std::to_string(signatures.size() - startIndex) + " transactions"
        );

        auto processed = 0;
        std::string lastProcessedSig = nullptr;

        for (int i = startIndex; i < signatures.size(); i++) {
            const auto sig = signatures[i];
            const auto tx = connection.getTransaction(sig.signature, {;
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
                        console.log(
                        "[Cron Solana] Detected token registration: " + sig.signature
                        );
                        // TODO: Parse and register token (Solana parsing not fully implemented)
                        // For now, just log it
                        processed++;
                        lastProcessedSig = sig.signature;
                    }
                }
            }

            if (lastProcessedSig) {
                lastSolanaSignature = lastProcessedSig;
            }

            return { processed, lastSignature: lastSolanaSignature }
            } catch (error) {
                std::cerr << "[Cron Solana] Error:" << error << std::endl;
                return {
                    processed: 0,
                    error: true /* instanceof check */ ? error.message : "Unknown error",
                    };
                }

}

std::future<void> GET(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Verify authorization
    const auto authHeader = request.headers.get("authorization");
    const auto cronSecret = std::getenv("CRON_SECRET");

    // Always require auth in production
    if (std::getenv("NODE_ENV") == "production" && !cronSecret) {
        std::cerr << "[Cron] No CRON_SECRET configured in production" << std::endl;
        return NextResponse.json(;
        { error: "Server configuration error" },
        { status: 500 },
        );
    }

    if (cronSecret && authHeader != "Bearer " + std::to_string(cronSecret) + "") {
        console.warn("[Cron] Unauthorized access attempt", {
            ip:
            request.headers.get("x-forwarded-for") ||;
            request.headers.get("x-real-ip"),
            timestamp: std::make_unique<Date>().toISOString(),
            });
            return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
        }

        std::cout << "[Cron] Starting token registration poll..." << std::endl;

        const auto results = {;
            base: {
                processed: 0,
                error: nullptr | nullptr,
                latestBlock: nullptr | nullptr,
                },
                solana: {
                    processed: 0,
                    error: nullptr | nullptr,
                    lastSignature: nullptr | nullptr,
                    },
                    timestamp: std::make_unique<Date>().toISOString(),
                    };

                    // Poll Base
                    try {
                        const auto baseResult = pollBaseRegistrations();
                        results.base = {
                            processed: baseResult.processed || 0,
                            error: baseResult.error || nullptr,
                            latestBlock: baseResult.latestBlock || nullptr,
                            };
                            } catch (error) {
                                results.base.error =;
                                true /* instanceof check */ ? error.message : "Unknown error";
                            }

                            // Poll Solana
                            try {
                                const auto solanaResult = pollSolanaRegistrations();
                                results.solana = {
                                    processed: solanaResult.processed || 0,
                                    error: solanaResult.error || nullptr,
                                    lastSignature: solanaResult.lastSignature || nullptr,
                                    };
                                    } catch (error) {
                                        results.solana.error =;
                                        true /* instanceof check */ ? error.message : "Unknown error";
                                    }

                                    const auto totalProcessed = results.base.processed + results.solana.processed;

                                    return NextResponse.json({;
                                        success: true,
                                        "message: " + "Processed " + totalProcessed + " new token registrations"
                                        results,
                                        });

}

} // namespace elizaos
