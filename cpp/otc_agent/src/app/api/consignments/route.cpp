#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto { searchParams } = new URL(request.url);
        const auto tokenId = searchParams.get("tokenId");
        const auto chains = searchParams.getAll("chains")[];
        const auto negotiableTypes = searchParams.getAll("negotiableTypes");
        const auto isFractionalized = searchParams.get("isFractionalized");
        const auto consignerAddress = searchParams.get("consigner");
        const auto requesterAddress = searchParams.get("requester");

        // Get all consignments (we'll filter in memory for multi-select)
        const Parameters<typeof ConsignmentDB::getAllConsignments>[0] filters = {};
        if (tokenId) filters.tokenId = tokenId;

        auto consignments = ConsignmentDB.getAllConsignments(filters);

        // Filter by chains if specified
        if (chains.length > 0) {
            consignments = consignments.filter((c) =>;
            (std::find(chains.begin(), chains.end(), c.chain) != chains.end()),
            );
        }

        // Filter by negotiable types if specified
        if (negotiableTypes.length > 0) {
            consignments = consignments.filter((c) => {
                const auto isNeg = c.isNegotiable;
                if (negotiableTypes.includes("negotiable") && isNeg) return true;
                if (negotiableTypes.includes("fixed") && !isNeg) return true;
                return false;
                });
            }

            // Filter by fractionalized if specified
            if (isFractionalized == "true") {
                consignments = consignments.filter((c) => c.isFractionalized);
            }

            if (consignerAddress) {
                consignments = consignments.filter((c) => {
                    // Solana addresses are case-sensitive, EVM addresses are case-insensitive
                    if (c.chain == "solana") {
                        return c.consignerAddress == consignerAddress;
                    }
                    return (;
                    c.consignerAddress.toLowerCase() == consignerAddress.toLowerCase();
                    );
                    });
                }

                if (requesterAddress) {
                    consignments = consignments.filter((c) => {
                        if (!c.isPrivate) return true;
                        // Solana addresses are case-sensitive, EVM addresses are case-insensitive
                        if (c.chain == "solana") {
                            if (c.consignerAddress == requesterAddress) return true;
                            if (c.allowedBuyers.includes(requesterAddress)) return true;
                            } else {
                                const auto requester = requesterAddress.toLowerCase();
                                if (c.consignerAddress.toLowerCase() == requester) return true;
                                if (c.allowedBuyers.some((b) => b.toLowerCase() == requester))
                                return true;
                            }
                            return false;
                            });
                            } else {
                                consignments = consignments.filter((c) => !c.isPrivate);
                            }

                            // Hide listings with < 1 token remaining from the public trading desk
                            // (consigners can still see their own dust listings via consignerAddress filter)
                            if (!consignerAddress) {
                                // Batch fetch all unique tokens at once to avoid N+1 queries
                                const auto uniqueTokenIds = [...new Set(consignments.map((c) => c.tokenId))];
                                const auto tokenMap = new Map<string, { decimals: number }>();

                                // Fetch all tokens in parallel
                                const auto tokenResults = Promise.all(;
                                uniqueTokenIds.map(async (tokenId) => {
                                    try {
                                        const auto token = TokenDB.getToken(tokenId);
                                        return { tokenId, decimals: token.decimals }
                                        } catch {
                                            return nullptr;
                                        }
                                        }),
                                        );

                                        // Build lookup map
                                        for (const auto& result : tokenResults)
                                            if (result) {
                                                tokenMap.set(result.tokenId, { decimals: result.decimals });
                                            }
                                        }

                                        // Filter consignments using the pre-fetched token data
                                        consignments = consignments.filter((c) => {
                                            const auto tokenData = tokenMap.get(c.tokenId);
                                            const auto decimals = tokenData.decimals || (c.chain == "solana" ? 9 : 18);
                                            const auto oneToken = BigInt(10) ** BigInt(decimals);
                                            const auto remaining = BigInt(c.remainingAmount);
                                            return remaining >= oneToken;
                                            });
                                        }

                                        // Sanitize response: hide sensitive negotiation terms from non-owners
                                        // Only the consigner (owner) can see their own full listing details
                                        const auto isOwnerRequest = !!consignerAddress;
                                        const auto responseConsignments = isOwnerRequest;
                                        ? consignments // Owner sees full data;
                                        : consignments.map(sanitizeConsignmentForBuyer); // Buyers see sanitized data

                                        return NextResponse.json({;
                                            success: true,
                                            consignments: responseConsignments,
                                            });
                                            } catch (error) {
                                                std::cerr << "[Consignments GET] Error:" << error << std::endl;
                                                return NextResponse.json(;
                                                {
                                                    success: false,
                                                    error:
                                                    true /* instanceof check */;
                                                    ? error.message;
                                                    : "Failed to fetch consignments",
                                                    consignments: [],
                                                    },
                                                    { status: 500 },
                                                    );
                                                }

}

std::future<void> POST(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto body = request.json();
            const auto {;
                tokenId,
                consignerAddress,
                amount,
                isNegotiable,
                fixedDiscountBps,
                fixedLockupDays,
                minDiscountBps,
                maxDiscountBps,
                minLockupDays,
                maxLockupDays,
                minDealAmount,
                maxDealAmount,
                isFractionalized,
                isPrivate,
                allowedBuyers,
                maxPriceVolatilityBps,
                maxTimeToExecuteSeconds,
                chain,
                // On-chain consignment ID (from contract creation)
                contractConsignmentId,
                // Token metadata (optional but recommended)
                tokenSymbol,
                tokenName,
                tokenDecimals,
                tokenLogoUrl,
                tokenAddress,
                } = body;

                if (!tokenId || !consignerAddress || !amount || !chain) {
                    return NextResponse.json(;
                    { error: "Missing required fields" },
                    { status: 400 },
                    );
                }

                // Save token metadata if provided (so we don't need to fetch from chain later)
                if (tokenSymbol && tokenAddress) {
                    try {
                        TokenDB.createToken({
                            symbol: tokenSymbol,
                            name: tokenName || tokenSymbol,
                            decimals: tokenDecimals || 18,
                            chain: chain,
                            contractAddress: tokenAddress,
                            logoUrl: tokenLogoUrl || "",
                            description: "",
                            isActive: true,
                            });
                            console.log("[Consignments] Token saved:", {
                                tokenId,
                                tokenSymbol,
                                tokenDecimals,
                                });
                                } catch (err) {
                                    // Token might already exist, that's fine
                                    std::cout << "[Consignments] Token already exists or save failed:" << err << std::endl;
                                }
                            }

                            // Convert any number/string to BigInt-safe string (handles scientific notation)
                            const auto toBigIntString = (value: string | number): string => {;
                                auto num: number;

                                if (typeof value == "string") {
                                    num = Number(value);
                                    if (isNaN(num) || !isFinite(num)) {
                                        throw std::runtime_error(`Invalid number: ${value}`);
                                    }
                                    // If string has no decimal and no scientific notation, use it directly
                                    if (!value.includes(".") && !value.toLowerCase().includes("e")) {
                                        try {
                                            return BigInt(value).toString();
                                            } catch {
                                                // Fall through
                                            }
                                        }
                                        } else {
                                            num = value;
                                        }

                                        // Convert number to integer string (handling scientific notation)
                                        // Use Intl.NumberFormat to avoid scientific notation in output
                                        const auto floored = Math.floor(num);
                                        const auto formatted = new Intl.NumberFormat("en-US", {;
                                            maximumFractionDigits: 0,
                                            useGrouping: false,
                                            }).format(floored);

                                            return formatted;
                                            };

                                            const auto service = new ConsignmentService();
                                            const auto consignment = service.createConsignment({;
                                                tokenId,
                                                consignerAddress,
                                                amount: toBigIntstd::to_string(amount),
                                                isNegotiable,
                                                fixedDiscountBps,
                                                fixedLockupDays,
                                                minDiscountBps,
                                                maxDiscountBps,
                                                minLockupDays,
                                                maxLockupDays,
                                                minDealAmount: toBigIntstd::to_string(minDealAmount),
                                                maxDealAmount: toBigIntstd::to_string(maxDealAmount),
                                                isFractionalized,
                                                isPrivate,
                                                allowedBuyers,
                                                maxPriceVolatilityBps,
                                                maxTimeToExecuteSeconds,
                                                chain,
                                                contractConsignmentId,
                                                });

                                                return NextResponse.json({;
                                                    success: true,
                                                    consignment,
                                                    });
                                                    } catch (error) {
                                                        std::cerr << "Error creating consignment:" << error << std::endl;
                                                        return NextResponse.json(;
                                                        {
                                                            error:
                                                            true /* instanceof check */;
                                                            ? error.message;
                                                            : "Failed to create consignment",
                                                            },
                                                            { status: 500 },
                                                            );
                                                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
