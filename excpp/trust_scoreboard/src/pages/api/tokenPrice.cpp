#include "tokenPrice.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> handler(NextApiRequest req, NextApiResponse res) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (req.method != 'GET') {
            return res.status(405).json({ error: 'Method not allowed' });
        }

        try {
            if (!HELIUS_API) {
                throw std::runtime_error('Helius API key not configured');
            }

            const auto response = "https://api.helius.xyz/v0/token-metadata?api-key=" + std::to_string(HELIUS_API);
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                    },
                    body: JSON.stringify({
                        mintAccounts: TOKEN_ADDRESSES,
                        includeOffChain: true,
                        disableCache: true,
                        }),
                        });

                        if (!response.ok) {
                            throw std::runtime_error(`Helius API error: ${response.status}`);
                        }

                        const auto data = response.json();

                        // Transform data to expected format
                        const std::vector<TokenPrice> prices = data.map((token: any) => ({;
                            address: token.account,
                            usdPrice: token.price || 0
                            }));

                            res.status(200).json({ prices });
                            } catch (error) {
                                std::cerr << 'Token price API error:' << error << std::endl;
                                // Return fallback prices if API fails
                                res.status(200).json({
                                    prices: TOKEN_ADDRESSES.map(address => ({
                                        address,
                                        usdPrice: 0
                                        }));
                                        });
                                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
