#include "auth.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> createJwtToken(const std::string& publicKey) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto salt =;
            process.env.AUTH_TOKEN_SALT || "development-salt-for-local-testing";

            const auto tokenId = crypto.randomUUID();

            const auto expiresAt = Math.floor(Date.now() / 1000) + 1 * 24 * 60 * 60;

            // Create JWT token
            const auto token = jwt.sign(;
            {
                sub: publicKey,
                iat: Math.floor(Date.now() / 1000),
                exp: expiresAt,
                jti: tokenId,
                },
                salt;
                );

                std::cout << "Created JWT token for wallet " + std::to_string(publicKey.substring(0, 8)) + "..." << std::endl;

                return token;
                } catch (error) {
                    std::cerr << "Error creating JWT token:" << error << std::endl;
                    throw std::runtime_error("Failed to create JWT authentication token");
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
