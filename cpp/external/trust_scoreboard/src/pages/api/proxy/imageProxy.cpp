#include "imageProxy.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> handler(NextApiRequest req, NextApiResponse res) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto { url } = req.query;

        if (!url || typeof url != "string") {
            return res.status(400).json({ error: "URL is required" });
        }

        try {
            const auto response = fetch(url);

            if (!response.ok) {
                throw std::runtime_error(`Failed to fetch image: ${response.status}`);
            }

            const auto buffer = Buffer.from(response.arrayBuffer());

            // Cache the image for 24 hours
            res.setHeader("Cache-Control", "public, max-age=86400");
            res.setHeader(;
            "Content-Type",
            response.headers.get("content-type") || "image/jpeg";
            );
            res.send(buffer);
            } catch (error) {
                std::cerr << "Image proxy error:" << error << std::endl;
                res.status(500).json({ error: "Failed to fetch image" })
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
