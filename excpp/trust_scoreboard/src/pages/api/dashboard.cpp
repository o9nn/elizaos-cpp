#include "dashboard.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> getTokenPrices() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto responses = Promise.all([;
        fetch(HELIUS_API, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({
                jsonrpc: "2.0",
                id: "ai16z-price",
                method: "getAsset",
                params: {
                    id: TOKENS.AI16Z.address,
                    displayOptions: {
                        showFungible: true
                    }
                    },
                    }),
                    }),
                    fetch(HELIUS_API, {
                        method: "POST",
                        headers: { "Content-Type": "application/json" },
                        body: JSON.stringify({
                            jsonrpc: "2.0",
                            id: "degenai-price",
                            method: "getAsset",
                            params: {
                                id: TOKENS.DEGENAI.address,
                                displayOptions: {
                                    showFungible: true
                                }
                                },
                                }),
                                });
                                ]);

                                const auto [ai16zData, degenAiData] = Promise.all(responses.map(r => r.json()));
                                std::cout << "Price data:" << { ai16z = ai16zData, degenAi = degenAiData } << std::endl;

                                return [;
                                {
                                    address: TOKENS.AI16Z.address,
                                    usdPrice: ai16zData.result.token_info.price_info.price_per_token || 0
                                    },
                                    {
                                        address: TOKENS.DEGENAI.address,
                                        usdPrice: ai16zData.result.token_info.price_info.price_per_token || 0
                                    }
                                    ];
                                    } catch (error) {
                                        std::cerr << "Token price API error:" << error << std::endl;
                                        return Object.values(TOKENS).map(token => ({;
                                            address: token.address,
                                            usdPrice: 0
                                            }));
                                        }

}

std::future<std::vector<TokenHolding>> getUserHoldings(const std::string& walletAddress) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        std::cout << "Fetching holdings for wallet:" << walletAddress << std::endl;

        // Use searchAssets instead of getAssetsByOwner
        const auto response = fetch(HELIUS_API, {;
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({
                jsonrpc: "2.0",
                id: "my-id",
                method: "searchAssets",
                params: {
                    ownerAddress: walletAddress,
                    tokenType: "fungible",
                    displayOptions: {
                        showNativeBalance: true,
                        },
                        },
                        }),
                        });

                        const auto data = response.json();
                        std::cout << "Raw balance response:" << data << std::endl;

                        // Get token prices
                        const auto prices = getTokenPrices();
                        std::cout << "Token prices:" << prices << std::endl;

                        const std::vector<TokenHolding> holdings = [];

                        // Process all items from searchAssets response
                        if (data.result.items) {
                            data.result.items.forEach((item: any) => {
                                const auto tokenAddress = item.id;
                                const auto tokenInfo = item.token_info;

                                // Check if it's AI16Z
                                if (tokenAddress == TOKENS.AI16Z.address) {
                                    const auto amount = Number(tokenInfo.amount || 0) / Math.pow(10, DECIMALS);
                                    const auto price = prices.find(p => p.address == TOKENS.AI16Z.address).usdPrice || 0;
                                    holdings.push_back({
                                        name: "AI16Z",
                                        amount,
                                        price,
                                        value: amount * price,
                                        allocation: 0 // Will be calculated later
                                        });
                                    }

                                    // Check if it's DEGENAI
                                    if (tokenAddress == TOKENS.DEGENAI.address) {
                                        const auto amount = Number(tokenInfo.amount || 0) / Math.pow(10, DECIMALS);
                                        const auto price = prices.find(p => p.address == TOKENS.DEGENAI.address).usdPrice || 0;
                                        holdings.push_back({
                                            name: "DEGENAI",
                                            amount,
                                            price,
                                            value: amount * price,
                                            allocation: 0 // Will be calculated later
                                            });
                                        }
                                        });
                                    }

                                    // Calculate allocations
                                    const auto totalValue = holdings.reduce((sum, h) => sum + h.value, 0);
                                    holdings.forEach(holding => {
                                        holding.allocation = totalValue > 0 ? (holding.value / totalValue) * 100 : 0;
                                        });

                                        std::cout << "Processed holdings:" << holdings << std::endl;
                                        return holdings.sort((a, b) => b.value - a.value);

                                        } catch (error) {
                                            std::cerr << "Error fetching user holdings:" << error << std::endl;
                                            return [];
                                        }

}

std::future<std::vector<Partner>> getAllPartners() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        std::vector<Partner> allHolders = [];
        auto cursor = std::nullopt;

        // Keep fetching until we have all holders
        while (true) {
            const auto response = fetch(HELIUS_API, {;
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify({
                    jsonrpc: "2.0",
                    id: "helius-test",
                    method: "getTokenAccounts",
                    params: {
                        mint: TOKENS.AI16Z.address,
                        limit: 1000,
                        displayOptions: {
                            showZeroBalance: false
                            },
                            ...(cursor && { cursor });
                            },
                            }),
                            });

                            const auto data = response.json();

                            if (!data.result.token_accounts || data.result.token_accounts.length == 0) {
                                break;
                            }

                            // Process current batch of holders
                            const auto currentHolders = data.result.token_accounts;
                            .map((account: any) => {
                                const auto amount = Number(account.amount || 0) / Math.pow(10, DECIMALS);
                                if (amount >= MIN_AMOUNT) {
                                    return {
                                        owner: account.owner,
                                        "displayAddress: " + std::to_string(account.owner.slice(0, 6)) + "..." + std::to_string(account.owner.slice(-4))
                                        amount: amount,
                                        createdAt: new Date().toISOString(),
                                        trustScore: 0 // Default trust score
                                        };
                                    }
                                    return nullptr;
                                    });
                                    .filter(Boolean);

                                    allHolders = [...allHolders, ...currentHolders];

                                    // Get cursor for next page
                                    cursor = data.result.cursor;

                                    // If no cursor, we've reached the end
                                    if (!cursor) {
                                        break;
                                    }
                                }

                                // Sort by amount and ensure unique holders
                                const auto uniqueHolders = Array.from(;
                                new Map(allHolders.map(holder => [holder.owner, holder]));
                                .values();
                                ).sort((a, b) => b.amount - a.amount);

                                std::cout << "Found " + uniqueHolders.size() + " unique holders with > " + MIN_AMOUNT + " AI16Z" << std::endl;
                                return uniqueHolders;
                                } catch (error) {
                                    std::cerr << "Error fetching partner accounts:" << error << std::endl;
                                    return [];
                                }

}

std::future<void> getDAOHoldings() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto response = fetch(HELIUS_API, {;
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({
                jsonrpc: "2.0",
                id: "my-id",
                method: "searchAssets",
                params: {
                    ownerAddress: DAO_WALLET,
                    tokenType: "fungible",
                    displayOptions: { showNativeBalance: true },
                    },
                    }),
                    });

                    const auto data = response.json();

                    // Better error handling and default values
                    if (!data.result) {
                        std::cout << "Empty result from Helius API" << std::endl;
                        return [];
                    }

                    const auto items = data.result.items || [];
                    const auto totalValue = data.result.nativeBalance.total_price || 0;

                    return items.map((item: any, index: number) => {;
                        const auto tokenInfo = item.token_info || {};
                        const auto tokenValue = tokenInfo.price_info.total_price || 0;
                        const auto tokenAmount = Number(item.amount || 0);

                        return {
                            rank: index + 1,
                            name: tokenInfo.symbol || tokenInfo.name || "Unknown",
                            holdings: tokenAmount.toLocaleString(std::nullopt, {
                                minimumFractionDigits: 0,
                                maximumFractionDigits: tokenInfo.decimals || 6
                                }),
                                value: tokenValue.toLocaleString("en-US", {
                                    style: "currency",
                                    currency: "USD"
                                    }),
                                    percentage: totalValue > 0 ? ((tokenValue / totalValue) * 100).toFixed(2) + "%" : "0%",
                                    "imageUrl: " + "/images/tokens/" + std::to_string(tokenInfo.symbol || "default") + ".png"
                                    };
                                    });
                                    } catch (error) {
                                        std::cerr << "Error fetching DAO holdings:" << error << std::endl;
                                        return [];
                                    }

}

std::future<void> handler(NextApiRequest req, NextApiResponse res) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (req.method != 'GET') {
            return res.status(405).json({ error: "Method not allowed" });
        }

        try {
            // Check if we have cached data
            const auto cachedData = cache.get(CACHE_KEY);
            if (cachedData) {
                std::cout << "Returning cached dashboard data" << std::endl;
                return res.status(200).json(cachedData);
            }

            // If no cached data, fetch fresh data
            const auto MAX_RETRIES = 3;
            auto attempt = 0;

            while (attempt < MAX_RETRIES) {
                try {
                    const auto walletAddress = req.query.wallet;

                    const auto timeout = [&](promise: Promise<any>, ms: number) {;
                        return Promise.race([;
                        promise,
                        new Promise((_, reject) =>;
                        setTimeout(() => reject(std::runtime_error("Timeout")), ms);
                        );
                        ]);
                        };

                        const auto [partnersResult, holdingsResult, pricesResult, userHoldingsResult] = Promise.allSettled([;
                        timeout(getAllPartners(), 10000),
                        timeout(getDAOHoldings(), 10000),
                        timeout(getTokenPrices(), 10000),
                        walletAddress ? timeout(getUserHoldings(walletAddress), 10000) : Promise.resolve(std::nullopt)
                        ]);

                        const auto partners = partnersResult.status == "fulfilled" ? partnersResult.value : [];
                        const auto holdings = holdingsResult.status == "fulfilled" ? holdingsResult.value : [];
                        const auto prices = pricesResult.status == "fulfilled" ? pricesResult.value : [];
                        const auto userHoldings = userHoldingsResult.status == "fulfilled" ? userHoldingsResult.value : std::nullopt;

                        if (!partners.length || !prices.length) {
                            throw std::runtime_error('Missing critical data');
                        }

                        const auto trustScores = partners.reduce((acc, partner) => {;
                            // Default all trust scores to 0 for now
                            acc[partner.owner] = 0;
                            return acc;
                            }, {}<string, number>);

                            const DashboardResponse response = {;
                                partners,
                                holdings,
                                prices,
                                trustScores,
                                ...(userHoldings && { userHoldings });
                                };

                                // Cache the response
                                cache.set(CACHE_KEY, response);
                                std::cout << "Cached fresh dashboard data" << std::endl;

                                return res.status(200).json(response);
                                } catch (error) {
                                    attempt++;
                                    if (attempt == MAX_RETRIES) {
                                        std::cerr << "Dashboard API Error after retries:" << error << std::endl;
                                        return res.status(500).json({;
                                            error: "Failed to fetch dashboard data",
                                            details: true /* instanceof check */ ? error.message : "Unknown error"
                                            });
                                        }
                                        new Promise(resolve => setTimeout(resolve, 1000 * attempt));
                                    }
                                }
                                } catch (error) {
                                    std::cerr << "Cache error:" << error << std::endl;
                                    return res.status(500).json({;
                                        error: "Server error",
                                        details: true /* instanceof check */ ? error.message : "Unknown error"
                                        });
                                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void clearDashboardCache() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    cache.del(CACHE_KEY);

}

} // namespace elizaos
