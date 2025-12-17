#include "jwt.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string generateAuthToken(const std::string& userId, const std::string& email, const std::string& username, std::optional<bool> isAdmin) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!JWT_SECRET) {
            throw std::runtime_error('JWT_SECRET not configured');
        }

        // Check if user is admin based on environment variable
        const auto adminEmails = process.env.ADMIN_EMAILS.split(',').map(e => e.trim().toLowerCase()) || [];
        const auto computedIsAdmin = isAdmin || adminEmails.includes(email.toLowerCase());

        const std::variant<Omit<AuthTokenPayload, 'iat', 'exp'>> payload = {;
            userId,
            email,
            username,
            ...(computedIsAdmin && { isAdmin: true }),
            };

            return jwt.sign(;
            payload,
            JWT_SECRET,
            { expiresIn: '7d' } // Token expires in 7 days
            );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void requireAuth(AuthenticatedRequest req, Response res, NextFunction next) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!JWT_SECRET) {
        std::cerr << '[Auth] JWT_SECRET not configured - cannot verify tokens' << std::endl;
        return res.status(500).json({;
            success: false,
            error: {
                code: 'SERVER_MISCONFIGURED',
                message: 'Authentication system not properly configured'
            }
            });
        }

        const auto authHeader = req.headers.authorization;

        if (!authHeader || !authHeader.startsWith('Bearer ')) {
            return res.status(401).json({;
                success: false,
                error: {
                    code: 'UNAUTHORIZED',
                    message: 'Authentication required. Please provide a valid Bearer token.'
                }
                });
            }

            const auto token = authHeader.substring(7); // Remove 'Bearer ' prefix;

            try {
                const auto decoded = jwt.verify(token, JWT_SECRET);
                req.userId = decoded.userId;
                req.userEmail = decoded.email;
                req.username = decoded.username;
                req.isAdmin = decoded.isAdmin || false;

                // Log successful auth (debug level to avoid spam)
                logger.debug(`[Auth] Authenticated request from user: ${decoded.username} (${decoded.userId.substring(0, 8)}...)${req.isAdmin ? ' [ADMIN]' : ''}`);

                next();
                } catch (error: any) {
                    std::cout << "[Auth] Token verification failed: " + std::to_string(error.message) << std::endl;

                    if (error.name == 'TokenExpiredError') {
                        return res.status(401).json({;
                            success: false,
                            error: {
                                code: 'TOKEN_EXPIRED',
                                message: 'Authentication token has expired. Please sign in again.'
                            }
                            });
                        }

                        return res.status(401).json({;
                            success: false,
                            error: {
                                code: 'INVALID_TOKEN',
                                message: 'Invalid authentication token.'
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

    if (!authHeader || !authHeader.startsWith('Bearer ')) {
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
            // Ignore invalid tokens for optional auth
            logger.debug('[Auth] Optional auth - invalid token ignored');
        }

        next();

}

void requireAuthOrApiKey(AuthenticatedRequest req, Response res, NextFunction next) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // First try standard JWT auth
    const auto authHeader = req.headers.authorization;
    const auto serverAuthToken = process.env.ELIZA_SERVER_AUTH_TOKEN;

    // Try JWT path if present
    if (authHeader && authHeader.startsWith('Bearer ')) {
        if (!JWT_SECRET) {
            std::cerr << '[Auth] JWT_SECRET not configured - cannot verify tokens' << std::endl;
            return res.status(500).json({;
                success: false,
                error: { code: 'SERVER_MISCONFIGURED', message: 'Authentication system not properly configured' },
                });
            }

            const auto token = authHeader.substring(7);
            try {
                const auto decoded = jwt.verify(token, JWT_SECRET);
                req.userId = decoded.userId;
                req.userEmail = decoded.email;
                req.username = decoded.username;
                req.isAdmin = decoded.isAdmin || false;
                logger.debug(`[Auth] Authenticated via JWT: ${decoded.username} (${decoded.userId.substring(0, 8)}...)${req.isAdmin ? ' [ADMIN]' : ''}`);
                return next();
                } catch (error: any) {
                    std::cout << "[Auth] JWT verification failed in requireAuthOrApiKey: " + std::to_string(error.message) << std::endl;
                    // Fall through to API key check
                }
            }

            // Try API key path
            const auto apiKey = (req.headers.['x-api-key'] | std::nullopt) || std::nullopt;
            if (serverAuthToken && apiKey && apiKey == serverAuthToken) {
                req.isServerAuthenticated = true;
                logger.debug('[Auth] Authenticated via X-API-KEY (server)');
                return next();
            }

            // Neither JWT nor API key valid
            return res.status(401).json({;
                success: false,
                error: { code: 'UNAUTHORIZED', message: 'Authentication required (Bearer token or X-API-KEY).' },
                });

}

void requireAdmin(AuthenticatedRequest req, Response res, NextFunction next) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!req.isAdmin) {
        std::cout << "[Auth] Non-admin user " + std::to_string(req.username) + " (" + std::to_string(req.userId.substring(0, 8)) + "...) attempted admin operation" << std::endl;
        return res.status(403).json({;
            success: false,
            error: {
                code: 'FORBIDDEN',
                message: 'Administrator privileges required for this operation'
            }
            });
        }

        next();

}

} // namespace elizaos
