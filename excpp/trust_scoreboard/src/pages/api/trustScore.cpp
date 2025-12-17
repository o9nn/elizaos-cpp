#include "trustScore.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> handler(NextApiRequest req, NextApiResponse res) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (req.method != 'GET') {
        return res.status(405).json({ error: "Method not allowed" });
    }

    try {
        // Mock data for example - replace with real data source
        const std::unordered_map<std::string, double> mockTrustScores = {;
            "0x123...": 750000,
            "0x456...": 250000,
            "0x789...": 100000,
            };

            const auto scores = Object.entries(mockTrustScores).reduce((acc, [address, amount]) => {;
                acc[address] = calculateTrustScore(amount).score;
                return acc;
                }, {}<string, number>);

                return res.status(200).json(scores);
                } catch (error) {
                    std::cerr << "Trust score calculation error:" << error << std::endl;
                    return res.status(500).json({ error: "Failed to calculate trust scores" });
                }

}

} // namespace elizaos
