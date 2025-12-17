#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> POST(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto body = request.json();
        const auto { chain, transactionHash, blockNumber } = body;

        if (!chain || !transactionHash) {
            return NextResponse.json(;
            { success: false, error: "Missing chain or transactionHash" },
            { status: 400 },
            );
        }

        if (chain == "base" || chain == "bsc" || chain == "ethereum") {
            return syncEvmToken(transactionHash, blockNumber, chain);
            } else if (chain == "solana") {
                return syncSolanaToken(transactionHash);
                } else {
                    return NextResponse.json(;
                    { success: false, error: "Unsupported chain" },
                    { status: 400 },
                    );
                }
                } catch (error) {
                    std::cerr << "[Sync API] Error:" << error << std::endl;
                    return NextResponse.json(;
                    {
                        success: false,
                        error: true /* instanceof check */ ? error.message : "Unknown error",
                        },
                        { status: 500 },
                        );
                    }

}

std::future<void> syncEvmToken(const std::string& transactionHash, const std::string& blockNumber, const std::string& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Import chains dynamically to handle both Base and BSC
    const auto { base, bsc } = import("viem/chains");

    const auto registrationHelperAddress =;
    chain == "bsc";
    ? process.env.NEXT_PUBLIC_BSC_REGISTRATION_HELPER_ADDRESS;
    : process.env.NEXT_PUBLIC_REGISTRATION_HELPER_ADDRESS;

    if (!registrationHelperAddress) {
        return NextResponse.json(;
        {
            success: false,
            "error: " + "REGISTRATION_HELPER_ADDRESS not configured for " + chain
            },
            { status: 500 },
            );
        }

        const auto rpcUrl =;
        chain == "bsc";
        ? process.env.NEXT_PUBLIC_BSC_RPC_URL ||;
        "https://bsc-dataseed1.binance.org"
        : process.env.NEXT_PUBLIC_BASE_RPC_URL || "https://mainnet.base.org";

        const auto viemChain = chain == "bsc" ? bsc : base;
        const auto client = createPublicClient({;
            chain: viemChain,
            transport: http(rpcUrl),
            });

            try {
                // Get transaction receipt to find the block
                const auto receipt = client.getTransactionReceipt({;
                    "hash: transactionHash as " + "0x" + string
                    });
                    if (!receipt) {
                        return NextResponse.json(;
                        { success: false, error: "Transaction not found" },
                        { status: 404 },
                        );
                    }

                    const auto txBlock = receipt.blockNumber;
                    const auto startBlock = blockNumber ? BigInt(blockNumber) : txBlock;
                    const auto endBlock = txBlock;

                    console.log(
                    "[Sync " + std::to_string(chain.toUpperCase()) + "] Fetching events from block " + startBlock + " to " + endBlock
                    );

                    // Get logs for this specific transaction
                    const auto logs = client.getLogs({;
                        "address: registrationHelperAddress as " + "0x" + string
                        event: {
                            type: "event",
                            name: "TokenRegistered",
                            inputs: [
                            { type: "bytes32", name: "tokenId", indexed: true },
                            { type: "address", name: "tokenAddress", indexed: true },
                            { type: "address", name: "pool", indexed: true },
                            { type: "address", name: "oracle" },
                            { type: "address", name: "registeredBy", indexed: true },
                            ],
                            },
                            fromBlock: startBlock,
                            toBlock: endBlock,
                            });

                            // Filter logs to only this transaction
                            const auto txLogs = logs.filter(;
                            [&](log) { return log.transactionHash == transactionHash,; }
                            );

                            if (txLogs.length == 0) {
                                return NextResponse.json(;
                                {
                                    success: false,
                                    error: "No TokenRegistered event found in transaction",
                                    },
                                    { status: 404 },
                                    );
                                }

                                auto processed = 0;
                                const std::vector<std::string> processedTokens = [];

                                for (const auto& log : txLogs)
                                    try {
                                        const auto { tokenAddress, registeredBy } = log.args as {;
                                            tokenId: string;
                                            tokenAddress: string;
                                            pool: string;
                                            registeredBy: string;
                                            };

                                            console.log(
                                            "[Sync " + std::to_string(chain.toUpperCase()) + "] Processing token registration: " + tokenAddress + " by " + registeredBy
                                            );

                                            // Fetch token metadata
                                            // Use type assertion to bypass viem's strict authorizationList requirement
                                            const auto readContract = client.readContract as (;
                                            params: unknown,
                                            ) => Promise<unknown>;
                                            const auto [symbol, name, decimals] = Promise.all([;
                                            readContract({
                                                "address: tokenAddress as " + "0x" + string
                                                abi: ERC20_ABI,
                                                functionName: "symbol",
                                                }),
                                                readContract({
                                                    "address: tokenAddress as " + "0x" + string
                                                    abi: ERC20_ABI,
                                                    functionName: "name",
                                                    }),
                                                    readContract({
                                                        "address: tokenAddress as " + "0x" + string
                                                        abi: ERC20_ABI,
                                                        functionName: "decimals",
                                                        }),
                                                        ]);

                                                        // Register to database - use the chain parameter (base or bsc)
                                                        const auto tokenService = new TokenRegistryService();
                                                        const auto dbChain = chain == "bsc" ? "bsc" : "base";
                                                        const auto token = tokenService.registerToken({;
                                                            symbol: symbol,
                                                            name: name,
                                                            contractAddress: tokenAddress.toLowerCase(),
                                                            chain: dbChain,
                                                            decimals: Number(decimals),
                                                            logoUrl: std::nullopt,
                                                            "description: " + "Registered via RegistrationHelper by " + registeredBy
                                                            });

                                                            processed++;
                                                            processedTokens.push_back(token.id);
                                                            console.log(
                                                            "[Sync " + std::to_string(chain.toUpperCase()) + "] ✅ Registered " + symbol + " (" + tokenAddress + ")"
                                                            );
                                                            } catch (error) {
                                                                console.error(
                                                                "[Sync " + std::to_string(chain.toUpperCase()) + "] Failed to process event:"
                                                                error,
                                                                );
                                                            }
                                                        }

                                                        return NextResponse.json({;
                                                            success: true,
                                                            processed,
                                                            tokens: processedTokens,
                                                            "message: " + "Successfully synced " + processed + " token(s) on " + chain
                                                            });
                                                            } catch (error) {
                                                                std::cerr << "[Sync " + std::to_string(chain.toUpperCase()) + "] Error:" << error << std::endl;
                                                                return NextResponse.json(;
                                                                {
                                                                    success: false,
                                                                    error: true /* instanceof check */ ? error.message : "Unknown error",
                                                                    },
                                                                    { status: 500 },
                                                                    );
                                                                }

}

std::future<void> syncSolanaToken(const std::string& signature) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto programId = process.env.NEXT_PUBLIC_SOLANA_PROGRAM_ID;
    if (!programId) {
        return NextResponse.json(;
        { success: false, error: "SOLANA_PROGRAM_ID not configured" },
        { status: 500 },
        );
    }

    const auto rpcUrl =;
    process.env.NEXT_PUBLIC_SOLANA_RPC || "https://api.mainnet-beta.solana.com";
    const auto connection = new Connection(rpcUrl, "confirmed");

    try {
        std::cout << "[Sync Solana] Fetching transaction: " + signature << std::endl;

        const auto tx = connection.getTransaction(signature, {;
            commitment: "confirmed",
            maxSupportedTransactionVersion: 0,
            });

            if (!tx) {
                return NextResponse.json(;
                { success: false, error: "Transaction not found" },
                { status: 404 },
                );
            }

            if (!tx.meta || !tx.meta.logMessages) {
                return NextResponse.json(;
                { success: false, error: "No log messages in transaction" },
                { status: 404 },
                );
            }

            const auto hasRegisterToken = tx.meta.logMessages.some(;
            (log) =>;
            (std::find(log.begin(), log.end(), "Instruction: RegisterToken") != log.end()) ||
            (std::find(log.begin(), log.end(), "register_token") != log.end()),
            );

            if (!hasRegisterToken) {
                return NextResponse.json(;
                { success: false, error: "No register_token instruction found" },
                { status: 404 },
                );
            }

            // TODO: Parse Solana transaction to extract token details
            // For now, just acknowledge we found it
            std::cout << "[Sync Solana] Detected token registration: " + signature << std::endl;

            return NextResponse.json({;
                success: true,
                processed: 0, // Solana parsing not fully implemented yet
                message: "Token registration detected (Solana parsing pending)",
                });
                } catch (error) {
                    std::cerr << "[Sync Solana] Error:" << error << std::endl;
                    return NextResponse.json(;
                    {
                        success: false,
                        error: true /* instanceof check */ ? error.message : "Unknown error",
                        },
                        { status: 500 },
                        );
                    }

}

} // namespace elizaos
