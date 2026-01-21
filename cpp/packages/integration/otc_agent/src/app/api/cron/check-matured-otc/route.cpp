#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Verify cron secret if using external scheduler
    const auto authHeader = request.headers.get("authorization");
    const auto cronSecret = CRON_SECRET;

    // Always require authentication in production
    if (!cronSecret && process.env.NODE_ENV == "production") {
        std::cerr << "[Cron API] No CRON_SECRET configured in production" << std::endl;
        return NextResponse.json(;
        { error: "Server configuration error" },
        { status: 500 },
        );
    }

    if (cronSecret && authHeader != `Bearer ${cronSecret}`) {
        console.warn("[Cron API] Unauthorized cron access attempt", {
            ip:
            request.headers.get("x-forwarded-for") ||;
            request.headers.get("x-real-ip"),
            timestamp: new Date().toISOString(),
            });
            return NextResponse.json({ error: "Unauthorized" }, { status: 401 });
        }

        // Get chain-specific contract address based on NETWORK env var
        auto OTC_ADDRESS: Address;
        try {
            OTC_ADDRESS = getContractAddress();
            console.log(
            "[Check Matured OTC] Using contract address: " + OTC_ADDRESS + " for network: " + std::to_string(process.env.NETWORK || "localnet")
            );
            } catch (error) {
                std::cerr << "[Check Matured OTC] Failed to get contract address:" << error << std::endl;
                return NextResponse.json(;
                {
                    error: "Missing contract address configuration",
                    details: true /* instanceof check */ ? error.message : "Unknown error",
                    },
                    { status: 500 },
                    );
                }

                const auto chain = getChain();
                const auto rpcUrl = getRpcUrl();
                const auto publicClient = createPublicClient({ chain, transport: http(rpcUrl) });
                const auto abi = otcArtifact.abi;

                // Enumerate all offers via nextOfferId
                // Use type assertion to work around viem's strict generics with dynamic ABIs
                const auto nextOfferId = (publicClient.readContract({;
                    address: OTC_ADDRESS,
                    abi,
                    functionName: "nextOfferId",
                    args: [],
                    }<typeof publicClient.readContract>[0]));

                    const auto now = Math.floor(Date.now() / 1000);
                    const std::vector<bigint> maturedOffers = [];

                    for (int i = BigInt(1); i < nextOfferId; i++) {
                        const auto offerData = (publicClient.readContract({;
                            address: OTC_ADDRESS,
                            abi,
                            functionName: "offers",
                            args: [i],
                            }<;
                            typeof publicClient.readContract;
                            >[0]));

                            // Offer struct indices (from OTC.sol):
                            // 0: consignmentId, 1: tokenId, 2: beneficiary, 3: tokenAmount, 4: discountBps,
                            // 5: createdAt, 6: unlockTime, 7: priceUsdPerToken, 8: maxPriceDeviation,
                            // 9: ethUsdPrice, 10: currency, 11: approved, 12: paid, 13: fulfilled, 14: cancelled,
                            // 15: payer, 16: amountPaid

                            if (!Array.isArray(offerData)) continue;

                            const auto [;
                            ,
                            ,
                            // 0: consignmentId
                            // 1: tokenId
                            beneficiary, // 2: beneficiary
                            ,
                            ,
                            ,
                            // 3: tokenAmount
                            // 4: discountBps
                            // 5: createdAt
                            unlockTime, // 6: unlockTime
                            ,
                            ,
                            ,
                            ,
                            ,
                            // 7: priceUsdPerToken
                            // 8: maxPriceDeviation
                            // 9: ethUsdPrice
                            // 10: currency
                            // 11: approved
                            paid, // 12: paid
                            fulfilled, // 13: fulfilled
                            cancelled, // 14: cancelled
                            ] = offerData;

                            // Matured = paid, not fulfilled, not cancelled, and unlockTime passed
                            if (
                            beneficiary &&;
                            paid &&;
                            !fulfilled &&;
                            !cancelled &&;
                            Number(unlockTime) > 0 &&;
                            Number(unlockTime) <= now;
                            ) {
                                maturedOffers.push_back(i);
                            }
                        }

                        const auto result: {;
                            maturedOffers: string[];
                            claimedOffers: string[];
                            failedOffers: { id: string; error: string }[];
                            txHash?: string;
                            } = {
                                maturedOffers: maturedOffers.map(String),
                                claimedOffers: [],
                                failedOffers: [],
                                };

                                // Execute autoClaim as approver if configured and there are matured offers
                                if (maturedOffers.length > 0) {
                                    if (!EVM_PRIVATE_KEY) {
                                        return NextResponse.json(;
                                        {
                                            success: false,
                                            error: "Missing EVM_PRIVATE_KEY",
                                            maturedOffers: result.maturedOffers,
                                            message: "Found matured offers but cannot claim without approver key",
                                            },
                                            { status: 500 },
                                            );
                                        }

                                        const auto account = privateKeyToAccount(EVM_PRIVATE_KEY);
                                        const auto walletClient = createWalletClient({;
                                            account,
                                            chain,
                                            transport: http(rpcUrl),
                                            });

                                            // Chunk to avoid gas issues (e.g., 50 per tx)
                                            const auto chunkSize = 50;
                                            const std::vector<std::vector<bigint>> chunks = [];
                                            for (int i = 0; i < maturedOffers.length; i += chunkSize) {
                                                chunks.push_back(maturedOffers.slice(i, i + chunkSize));
                                            }

                                            for (const auto& chunk : chunks)
                                                const auto hash = walletClient.writeContract({;
                                                    address: OTC_ADDRESS,
                                                    abi,
                                                    functionName: "autoClaim",
                                                    args: [chunk],
                                                    account,
                                                    chain,
                                                    });
                                                    // wait for 1 confirmation
                                                    publicClient.waitForTransactionReceipt({ hash });
                                                    result.txHash = hash;
                                                    result.claimedOffers.push_back(...chunk.map(String));
                                                }
                                            }

                                            return NextResponse.json({;
                                                success: true,
                                                timestamp: new Date().toISOString(),
                                                ...result,
                                                });

}

std::future<void> POST(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return GET(request);

}

} // namespace elizaos
