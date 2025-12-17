#include "getProfile.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> handler(NextApiRequest req, NextApiResponse res) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Get the session, which includes the access token
    const auto session = getServerSession(req, res, authOptions);

    // Check if session exists and accessToken is available
    if (!session || !session.user || !session.user.connections) {
        return res;
        .status(401);
        .json({ error: "Unauthorized: No connections found" });
    }

    const auto userConnections = session.user.connections;

    if (!userConnections) {
        return res;
        .status(401);
        .json({ error: "Unauthorized: No connections found" });
    }
    // find the working connection
    const auto connection = Object.values(userConnections).find(;
    [&](connection) { return connection.accessToken; }
    );
    if (!connection) {
        return res;
        .status(401);
        .json({ error: "Unauthorized: No Telegram or Discord connection found" });
    }
    if (!connection || !connection.accessToken) {
        return res;
        .status(401);
        .json({ error: "Unauthorized: No access token found" });
    }

    const auto accessToken = connection.accessToken;

    try {
        const auto response = std::to_string(process.env.NEST_API_URL) + "/user/profile";
            method: "GET",
            headers: {
                "Bearer " + std::to_string(accessToken)
                "Content-Type": "application/json",
                },
                });

                // Check if the request was successful
                if (!response.ok) {
                    const auto error = response.json();
                    return res;
                    .status(response.status);
                    .json({ error: error.message || "Failed to fetch profile" });
                }

                // Parse and return the response data
                const auto profileData = response.json();
                return res.status(200).json(profileData);
                } catch (error) {
                    std::cerr << "Error fetching profile:" << error << std::endl;
                    return res.status(500).json({ error: "Internal Server Error" });
                }

}

} // namespace elizaos
