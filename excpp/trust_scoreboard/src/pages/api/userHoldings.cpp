#include "userHoldings.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> handler(NextApiRequest req, NextApiResponse res) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (req.method != "GET") {
            return res.status(405).json({ error: "Method not allowed" });
        }

        const auto walletAddress = req.query.wallet;
        if (!walletAddress) {
            return res.status(400).json({ error: "Wallet address is required" });
        }

        try {
            std::cout << "Fetching holdings for wallet:" << walletAddress << std::endl;

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

                            if (!response.ok || data.error) {
                                throw std::runtime_error(data.error?.message || "Failed to fetch token data");
                            }

                            if (!data.result.items) {
                                return res.status(200).json({ holdings: [] });
                            }

                            const auto holdings = data.result.items;
                            .map((item: any) => {
                                const auto tokenInfo = item.token_info || {};
                                const auto decimals =;
                                tokenInfo.decimals != std::nullopt ? tokenInfo.decimals : 9; // Default to 9 decimals if not specified
                                // Use balance instead of amount
                                const auto rawAmount = tokenInfo.balance || tokenInfo.amount || 0;
                                const auto amount = Number(rawAmount) / Math.pow(10, decimals);
                                const auto price = tokenInfo.price_info.price_per_token || 0;
                                const auto value = amount * price;

                                return {
                                    name: tokenInfo.symbol || tokenInfo.name || item.id,
                                    amount,
                                    price,
                                    value,
                                    allocation: 0,
                                    };
                                    });
                                    .filter((holding) => holding.amount > 0);

                                    if (holdings.length == 0) {
                                        std::cout << "No holdings found after processing." << std::endl;
                                    }

                                    const auto totalValue = holdings.reduce((sum, h) => sum + h.value, 0);
                                    holdings.forEach((h) => {
                                        h.allocation = totalValue > 0 ? (h.value / totalValue) * 100 : 0;
                                        });

                                        // Sort by value descending
                                        holdings.sort((a, b) => b.value - a.value);
                                        // console.log("Holdings:", holdings);

                                        return res.status(200).json({ holdings });
                                        } catch (error) {
                                            std::cerr << "API error:" << error << std::endl;
                                            return res.status(500).json({;
                                                error:
                                                true /* instanceof check */ ? error.message : "Failed to fetch holdings",
                                                });
                                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
