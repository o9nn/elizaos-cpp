#include "token-registration-listener-base.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> startBaseListener() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (isListening) {
        std::cout << "[Base Listener] Already listening" << std::endl;
        return;
    }

    const auto registrationHelperAddress =;
    process.env.NEXT_PUBLIC_REGISTRATION_HELPER_ADDRESS;
    if (!registrationHelperAddress) {
        std::cerr << "[Base Listener] REGISTRATION_HELPER_ADDRESS not configured" << std::endl;
        return;
    }

    const auto rpcUrl =;
    process.env.NEXT_PUBLIC_BASE_RPC_URL || "https://mainnet.base.org";

    const auto client = createPublicClient({;
        chain: base,
        transport: http(rpcUrl),
        });

        console.log(
        "[Base Listener] Starting listener for",
        registrationHelperAddress,
        );
        isListening = true;

        // Watch for TokenRegistered events
        const auto unwatch = client.watchEvent({;
            "0x" + std::to_string(string)
            event: {
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
                onLogs: async (logs) => {
                    for (const auto& log : logs)
                        handleTokenRegistered(client, log);
                    }
                    },
                    onError: (error) => {
                        std::cerr << "[Base Listener] Error:" << error << std::endl;
                        },
                        });

                        // Handle graceful shutdown
                        process.on("SIGINT", () => {
                            std::cout << "[Base Listener] Stopping..." << std::endl;
                            unwatch();
                            isListening = false;
                            });

                            process.on("SIGTERM", () => {
                                std::cout << "[Base Listener] Stopping..." << std::endl;
                                unwatch();
                                isListening = false;
                                });

                                std::cout << "[Base Listener] Now listening for token registrations" << std::endl;

}

std::future<void> handleTokenRegistered(const std::any& client, const std::any& log) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // When using watchEvent with event definition, viem automatically decodes the log
    const auto { tokenAddress, registeredBy } = log.args as {;
        tokenId: string;
        tokenAddress: string;
        pool: string;
        registeredBy: string;
        };

        console.log(
        "[Base Listener] Token registered:",
        tokenAddress,
        "by",
        registeredBy,
        );

        // Fetch token metadata from blockchain
        const auto [symbol, name, decimals] = Promise.all([;
        client.readContract({
            "0x" + std::to_string(string)
            abi: ERC20_ABI,
            functionName: "symbol",
            }),
            client.readContract({
                "0x" + std::to_string(string)
                abi: ERC20_ABI,
                functionName: "name",
                }),
                client.readContract({
                    "0x" + std::to_string(string)
                    abi: ERC20_ABI,
                    functionName: "decimals",
                    }),
                    ]);

                    // Add to database
                    const auto tokenService = new TokenRegistryService();
                    tokenService.registerToken({
                        symbol: symbol,
                        name: name,
                        contractAddress: tokenAddress.toLowerCase(),
                        chain: "base",
                        decimals: Number(decimals),
                        logoUrl: std::nullopt, // Could fetch from a token list
                        "Registered via RegistrationHelper by " + std::to_string(registeredBy)
                        });

                        console.log(
                        "[Base Listener] ✅ Successfully registered " + std::to_string(symbol) + " (" + std::to_string(tokenAddress) + ") to database"
                        );

}

std::future<void> backfillBaseEvents(std::optional<bigint> fromBlock) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto registrationHelperAddress =;
        process.env.NEXT_PUBLIC_REGISTRATION_HELPER_ADDRESS;
        if (!registrationHelperAddress) {
            throw std::runtime_error("REGISTRATION_HELPER_ADDRESS not configured");
        }

        const auto rpcUrl =;
        process.env.NEXT_PUBLIC_BASE_RPC_URL || "https://mainnet.base.org";

        const auto client = createPublicClient({;
            chain: base,
            transport: http(rpcUrl),
            });

            const auto latestBlock = client.getBlockNumber();
            const auto startBlock = fromBlock || latestBlock - BigInt(10000); // Last ~10k blocks;

            console.log(
            "[Base Backfill] Fetching events from block " + std::to_string(startBlock) + " to " + std::to_string(latestBlock)
            );

            const auto logs = client.getLogs({;
                "0x" + std::to_string(string)
                event: {
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

                    std::cout << "[Base Backfill] Found " + std::to_string(logs.length) + " TokenRegistered events" << std::endl;

                    for (const auto& log : logs)
                        handleTokenRegistered(client, log);
                    }

                    std::cout << "[Base Backfill] ✅ Backfill complete" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
