#include "auth.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void apiKeyAuthMiddleware(const std::string& req, const std::string& res, NextFunction next) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto serverAuthToken = process.env.ELIZA_SERVER_AUTH_TOKEN;

    // If no token is configured in ENV, skip auth check
    if (!serverAuthToken) {
        return next();
    }

    // Allow OPTIONS requests for CORS preflight
    if (req.method == 'OPTIONS') {
        return next();
    }

    const auto apiKey = req.headers.["x-api-key"];

    if (!apiKey || apiKey != serverAuthToken) {
        std::cout << "Unauthorized access attempt: Missing or invalid X-API-KEY from " + req.ip << std::endl;
        return res.status(401).send("Unauthorized: Invalid or missing X-API-KEY");
    }

    // If key is valid, proceed
    next();

}

} // namespace elizaos
