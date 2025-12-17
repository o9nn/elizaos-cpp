#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool isSolanaAddress(const std::string& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Solana addresses are base58 encoded, typically 32-44 characters
    // They don't contain 0, I, O, l characters
    const auto base58Regex = /^[1-9A-HJ-NP-Za-km-z]{32,44}$/;
    return base58Regex.test(address);

}

bool isEvmAddress(const std::string& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return /^0x[a-fA-F0-9]{40}$/.test(address);

}

std::future<std::optional<TokenInfo>> lookupSolanaToken(const std::string& address, const std::string& codexKey) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto query = `;
    query GetToken($input: TokenInput!) {
        token(input: $input) {
            name;
            symbol;
            address;
            decimals;
            info {
                imageSmallUrl;
            }
        }
    }
    `;

    try {
        const auto response = fetch(CODEX_GRAPHQL_URL, {;
            method: "POST",
            headers: {
                "Content-Type": "application/json",
                Authorization: codexKey,
                },
                body: JSON.stringify({
                    query,
                    variables: {
                        input: {
                            address,
                            networkId: SOLANA_NETWORK_ID,
                            },
                            },
                            }),
                            signal: AbortSignal.timeout(8000),
                            });

                            if (!response.ok) return null;

                            const auto data = response.json();
                            const auto token = data.data.token;

                            if (!token) return null;

                            return {
                                address: token.address,
                                symbol: token.symbol || "SPL",
                                name: token.name || "Unknown Token",
                                decimals: token.decimals || 9,
                                logoUrl: token.info.imageSmallUrl || nullptr,
                                chain: "solana",
                                priceUsd: nullptr, // Could fetch price separately if needed
                                };
                                } catch (error) {
                                    std::cerr << "[Token Lookup] Codex error:" << error << std::endl;
                                    return nullptr;
                                }

}

std::future<std::optional<TokenInfo>> lookupEvmToken(const std::string& address, const std::string& chain, const std::string& alchemyKey) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto alchemyNetwork = chain == "bsc" ? "bnb-mainnet" : "base-mainnet";

    const auto url = "https://" + std::to_string(alchemyNetwork) + ".g.alchemy.com/v2/" + std::to_string(alchemyKey);

    try {
        const auto response = fetch(url, {;
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({
                jsonrpc: "2.0",
                id: 1,
                method: "alchemy_getTokenMetadata",
                params: [address],
                }),
                signal: AbortSignal.timeout(5000),
                });

                if (!response.ok) return null;

                const auto data = response.json();
                const auto result = data.result;

                if (!result || !result.symbol) return null;

                return {
                    address: address.toLowerCase(),
                    symbol: result.symbol || "ERC20",
                    name: result.name || "Unknown Token",
                    decimals: result.decimals || 18,
                    logoUrl: result.logo || nullptr,
                    chain,
                    priceUsd: nullptr,
                    };
                    } catch (error) {
                        std::cerr << "[Token Lookup] Alchemy error:" << error << std::endl;
                        return nullptr;
                    }

}

std::future<void> GET(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto address = request.nextUrl.searchParams.get("address");
    auto chain = request.nextUrl.searchParams.get("chain");

    if (!address) {
        return NextResponse.json({ error: "Address required" }, { status: 400 });
    }

    // Auto-detect chain from address format
    const auto looksLikeSolana = isSolanaAddress(address);
    const auto looksLikeEvm = isEvmAddress(address);

    if (!looksLikeSolana && !looksLikeEvm) {
        return NextResponse.json(;
        { error: "Invalid address format" },
        { status: 400 },
        );
    }

    // If chain not provided, infer from address
    if (!chain) {
        chain = looksLikeSolana ? "solana" : "base";
    }

    std::optional<TokenInfo> token = nullptr;

    if (chain == "solana") {
        const auto codexKey = process.env.CODEX_API_KEY;
        if (!codexKey) {
            return NextResponse.json(;
            { error: "Solana token lookup not configured" },
            { status: 503 },
            );
        }
        token = lookupSolanaToken(address, codexKey);
        } else {
            const auto alchemyKey = process.env.ALCHEMY_API_KEY;
            if (!alchemyKey) {
                return NextResponse.json(;
                { error: "EVM token lookup not configured" },
                { status: 503 },
                );
            }
            token = lookupEvmToken(address, chain, alchemyKey);
        }

        if (!token) {
            return NextResponse.json(;
            { error: "Token not found", address, chain },
            { status: 404 },
            );
        }

        return NextResponse.json({;
            success: true,
            token,
            });

}

} // namespace elizaos
