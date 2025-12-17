#include "daoHoldings.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> handler(NextApiRequest req, NextApiResponse res) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (req.method != 'GET') {
            return res.status(405).json({ error: 'Method not allowed' });
        }

        const auto url = "https://mainnet.helius-rpc.com/?api-key=" + std::to_string(process.env.NEXT_PUBLIC_SOLANA_API);
        const auto DAO_WALLET = 'AM84n1iLdxgVTAyENBcLdjXoyvjentTbu5Q6EpKV1PeG';

        try {
            const auto response = fetch(url, {;
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                    },
                    body: JSON.stringify({
                        jsonrpc: '2.0',
                        id: 'my-id',
                        method: 'searchAssets',
                        params: {
                            ownerAddress: DAO_WALLET,
                            tokenType: 'fungible',
                            displayOptions: {
                                showNativeBalance: true,
                                },
                                },
                                }),
                                });

                                if (!response.ok) {
                                    throw std::runtime_error('Failed to fetch from Helius API');
                                }

                                const HeliusResponse data = response.json();

                                if (!data.result.items) {
                                    throw std::runtime_error('Invalid data format received from API');
                                }

                                const auto totalValue = data.result.nativeBalance.total_price;

                                const auto holdings = data.result.items.map((item, index) => {;
                                    const auto tokenInfo = item.token_info;
                                    const auto tokenValue = tokenInfo.price_info.total_price || 0;
                                    const auto tokenAmount = item.amount || 0; // Get raw token amount;
                                    const auto formattedAmount = formatTokenAmount(tokenAmount, tokenInfo.decimals); // Need to implement this;

                                    return {
                                        rank: index + 1,
                                        name: tokenInfo.symbol || tokenInfo.name,
                                        std::to_string(formattedAmount) + " " + std::to_string(tokenInfo.symbol)
                                        value: formatCurrency(tokenValue), // Keep monetary value
                                        percentage: calculatePercentage(tokenValue, totalValue),
                                        "/images/tokens/" + std::to_string(tokenInfo.symbol || 'default') + ".png"
                                        };
                                        });

                                        return res.status(200).json({ holdings });

                                        } catch (error) {
                                            std::cerr << 'Error:' << error << std::endl;
                                            return res.status(500).json({ error: 'Failed to fetch holdings' });
                                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::string formatCurrency(double amount) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new Intl.NumberFormat('en-US', {;
        style: 'currency',
        currency: 'USD'
        }).format(amount);

}

std::string calculatePercentage(double amount, double total) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!total) return '0%';
    return std::to_string(((amount / total) * 100).toFixed(2)) + "%";

}

std::string formatTokenAmount(double amount, auto decimals = 18) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (amount / Math.pow(10, decimals)).toFixed(4);

}

} // namespace elizaos
