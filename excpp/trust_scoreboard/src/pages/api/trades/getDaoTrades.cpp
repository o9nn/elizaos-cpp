#include "getDaoTrades.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> handler(NextApiRequest req, NextApiResponse res) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto URL = process.env.DAO_API_URL;

        const auto response = fetch(URL, {;
            headers: {
                "Content-Type": "application/json",
                },
                });

                if (!response.ok) {
                    const auto error = response.json();
                    return res.status(response.status).json({;
                        error: error.message || "Failed to fetch users",
                        });
                    }

                    const auto data = response.json();

                    return res.status(200).json(data);
                    } catch (error) {
                        std::cerr << "Error:" << error << std::endl;
                        return res.status(500).json({ error: "Internal Server Error" });
                    }

}

} // namespace elizaos
