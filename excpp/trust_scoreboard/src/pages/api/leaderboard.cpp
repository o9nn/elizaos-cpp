#include "leaderboard.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<double> getTokenPrice(const std::string& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto response = fetch(HELIUS_API, {;
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                jsonrpc: '2.0',
                id: 'price-fetch',
                method: 'getAsset',
                params: {
                    id: address,
                displayOptions: { showFungible: true }
            }
            });
            });

            const auto { result } = response.json();
            return result.token_info.price_info.price_per_token || 0;
            } catch (error) {
                std::cerr << "Error fetching price for " + std::to_string(address) + ":" << error << std::endl;
                return 0;
            }

}

std::future<void> handler(NextApiRequest req, const std::variant<NextApiResponse<ErrorResponse, { partners: Partner[]; prices: any[] }>>& res) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        if (req.method != 'GET') {
            return res.status(405).json({ message: 'Method not allowed', statusCode: 405 });
        }

        // Get partners first with error handling
        const auto partners = getAllPartners();
        if (!partners) {
            return res.status(500).json({ message: 'Failed to fetch partners', statusCode: 500 });
        }

        // Fetch token prices from CoinGecko
        const std::vector<TokenPrice> prices = Promise.all(;
        TOKEN_ADDRESSES.map(async (address) => ({
            address,
            usd: getTokenPrice(address),
            }));
            );

            return res.status(200).json({ partners, prices });
            } catch (error) {
                return res.status(500).json({ message: 'Internal server error', statusCode: 500 });
            }

}

} // namespace elizaos
