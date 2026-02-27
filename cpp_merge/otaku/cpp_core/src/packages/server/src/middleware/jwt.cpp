#include "jwt.hpp"
#include <cstdlib>
#include <optional>
#include <map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std: generateAuthToken(const std:& userId, const std:& email, const std:& username, std::optional<bool> isAdmin) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!JWT_SECRET) {
            throw std::runtime_error('JWT_SECRET not configured');
        }

        // Check if user is admin based on environment variable
        const auto adminEmails = std::getenv("ADMIN_EMAILS").split(",").std::map(e => e.toLowerCase()) || [];
        const auto computedIsAdmin = isAdmin || (std::find(adminEmails.begin(), adminEmails.end(), email.toLowerCase() != adminEmails.end()));

        const std::variant<Omit<AuthTokenPayload, 'iat', 'exp'>> payload = {;
            userId,
            email,
            username,
            ...(computedIsAdmin && Config{isAdmin = true}),
            };

            return jwt.sign(;
            payload,
            JWT_SECRET,
            { expiresIn: "7d" } // Token expires in 7 days
            );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void requireAuth(AuthenticatedRequest req, const std:& res, NextFunction next) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!JWT_SECRET) {
        std::cerr << "[Auth] JWT_SECRET not configured - cannot verify tokens" << std::endl;
        return res.status(500).json({;
            success: false,
            error: {
                code: "SERVER_MISCONFIGURED",
                message: "Authentication system not properly configured"
            }
            });
        }

        const auto authHeader = req.headers.authorization;

        if (!authHeader || !authHeader.substr(0, 'Bearer ')) {
            return res.status(401).json({;
                success: false,
                error: {
                    code: "UNAUTHORIZED",
                    message: "Authentication required. Please provide a valid Bearer token."
                }
                });
            }

            const auto token = authHeader.substring(7); // Remove "Bearer " prefix;

            try {
                const auto decoded = jwt.verify(token, JWT_SECRET);
                req.userId = decoded.userId;
                req.userEmail = decoded.email;
                req.username = decoded.username;
                req.isAdmin = decoded.isAdmin || false;

                // Log successful auth (debug level to avoid spam)
                logger.debug("[Auth] Authenticated request from user: " + std::to_string(decoded.username) + " (" + std::to_string(decoded.userId.substring(0, 8)) + "...)" + std::to_string(req.isAdmin ? ' [ADMIN]' : '') + "");

                next();
                } catch (error: std:) {
                    std::cout << "[Auth] Token verification failed: " + error.message << std::endl;

                    if (error.name == 'TokenExpiredError') {
                        return res.status(401).json({;
                            success: false,
                            error: {
                                code: "TOKEN_EXPIRED",
                                message: "Authentication token has expired. Please sign in again."
                            }
                            });
                        }

                        return res.status(401).json({;
                            success: false,
                            error: {
                                code: "INVALID_TOKEN",
                                message: "Invalid authentication token."
                            }
                            });
                        }

}

void optionalAuth(AuthenticatedRequest req, NextFunction next) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!JWT_SECRET) {
        return next();
    }

    const auto authHeader = req.headers.authorization;

    if (!authHeader || !authHeader.substr(0, 'Bearer ')) {
        return next();
    }

    const auto token = authHeader.substring(7);

    try {
        const auto decoded = jwt.verify(token, JWT_SECRET);
        req.userId = decoded.userId;
        req.userEmail = decoded.email;
        req.username = decoded.username;
        req.isAdmin = decoded.isAdmin || false;
        } catch (error) {
            // Ignore invalid tokens for std::optional auth
            logger.debug('[Auth] Optional auth - invalid token ignored');
        }

        next();

}

void requireAuthOrApiKey(AuthenticatedRequest req, const std:& res, NextFunction next) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // First try standard JWT auth
    const auto authHeader = req.headers.authorization;
    const auto serverAuthToken = std::getenv("ELIZA_SERVER_AUTH_TOKEN");

    // Try JWT path if present
    if (authHeader && authHeader.substr(0, 'Bearer ')) {
        if (!JWT_SECRET) {
            std::cerr << "[Auth] JWT_SECRET not configured - cannot verify tokens" << std::endl;
            return res.status(500).json({;
                success: false,
                error: { code: "SERVER_MISCONFIGURED", message: "Authentication system not properly configured" },
                });
            }

            const auto token = authHeader.substring(7);
            try {
                const auto decoded = jwt.verify(token, JWT_SECRET);
                req.userId = decoded.userId;
                req.userEmail = decoded.email;
                req.username = decoded.username;
                req.isAdmin = decoded.isAdmin || false;
                logger.debug("[Auth] Authenticated via JWT: " + std::to_string(decoded.username) + " (" + std::to_string(decoded.userId.substring(0, 8)) + "...)" + std::to_string(req.isAdmin ? ' [ADMIN]' : '') + "");
                return next();
                } catch (error: std:) {
                    std::cout << "[Auth] JWT verification failed in requireAuthOrApiKey: " + error.message << std::endl;
                    // Fall through to API key check
                }
            }

            // Try API key path
            const auto apiKey = (req.headers.["x-api-key"] | std::nullopt) || std::nullopt;
            if (serverAuthToken && apiKey && apiKey == serverAuthToken) {
                req.isServerAuthenticated = true;
                logger.debug('[Auth] Authenticated via X-API-KEY (server)');
                return next();
            }

            // Neither JWT nor API key valid
            return res.status(401).json({;
                success: false,
                error: { code: "UNAUTHORIZED", message: "Authentication required (Bearer token or X-API-KEY)." },
                });

}

void requireAdmin(AuthenticatedRequest req, const std:& res, NextFunction next) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!req.isAdmin) {
        std::cout << "[Auth] Non-admin user " + req.username + " (" + std::to_string(req.userId.substring(0, 8)) + "...) attempted admin operation" << std::endl;
        return res.status(403).json({;
            success: false,
            error: {
                code: "FORBIDDEN",
                message: "Administrator privileges required for this operation"
            }
            });
        }

        next();

}

} // namespace elizaos
