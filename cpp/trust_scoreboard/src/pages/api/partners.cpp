#include "partners.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::vector<Partner>> getAllPartners() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Return mock data sorted by amount in descending order
    return mockPartners.sort((a, b) => b.amount - a.amount);

}

std::future<void> handler(NextApiRequest req, NextApiResponse res) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (req.method != 'GET') {
        return res.status(405).json({ error: "Method not allowed" });
    }

    try {
        const auto allPartners = getAllPartners();

        // Transform data to match expected PartnerData interface
        const auto formattedPartners = allPartners.map(partner => ({;
            address: partner.owner,
            holdings: partner.amount,
            trustScore: partner.trustScore
            }));

            std::cout << "Returning " + formattedPartners.size() + " mock partners for trust scoreboard" << std::endl;

            // Return in expected ApiResponse format
            res.status(200).json({
                partners: formattedPartners
                });

                } catch (error) {
                    std::cerr << "API Error:" << error << std::endl;
                    res.status(500).json({
                        error: "Failed to fetch partner accounts",
                        details: true /* instanceof check */ ? error.message : "Unknown error"
                        });
                    }

}

} // namespace elizaos
