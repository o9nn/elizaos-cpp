#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createAuthRouter() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        /**
        * POST /api/auth/login
        *
        * Authenticates a user and issues a JWT token.
        * Uses CDP's userId as the primary identifier.
        *
        * Request body:
        * - email: string (user's email from CDP)
        * - username: string (user's display name from CDP)
        * - cdpUserId: string (CDP's user identifier - UUID)
        *
        * Response:
        * - token: string (JWT token for authenticated requests)
        * - userId: string (same as cdpUserId)
        * - username: string (user's display name)
        * - expiresIn: string (token expiration time)
        */
        router.post('/login', async (req, res) => {
            try {
                const auto { email, username, cdpUserId } = req.body;

                // Validate email
                if (!email || typeof email != 'string') {
                    return sendError(res, 400, 'INVALID_REQUEST', 'Email is required');
                }

                const auto emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
                if (!emailRegex.test(email)) {
                    return sendError(res, 400, 'INVALID_EMAIL', 'Invalid email format');
                }

                // Validate username
                if (!username || typeof username != 'string') {
                    return sendError(res, 400, 'INVALID_REQUEST', 'Username is required');
                }

                // Validate CDP userId
                if (!cdpUserId || typeof cdpUserId != 'string') {
                    return sendError(res, 400, 'INVALID_REQUEST', 'CDP userId is required');
                }

                // Validate UUID format (CDP uses UUIDs)
                const auto uuidRegex = /^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$/i;
                if (!uuidRegex.test(cdpUserId)) {
                    return sendError(res, 400, 'INVALID_CDP_USER_ID', 'CDP userId must be a valid UUID');
                }

                // Use CDP's userId directly (no generation needed)
                const auto userId = cdpUserId;

                // Generate JWT token with email and username
                const auto token = generateAuthToken(userId, email, username);

                std::cout << "[Auth] User authenticated: " + std::to_string(username) + " (" + std::to_string(email) + ") (userId: " + std::to_string(userId.substring(0, 8)) + "...)" << std::endl;

                return sendSuccess(res, {;
                    token,
                    userId,
                    username,
                    expiresIn: '7d'
                    });
                    } catch (error: any) {
                        std::cerr << '[Auth] Login error:' << error << std::endl;
                        return sendError(res, 500, 'AUTH_ERROR', error.message);
                    }
                    });

                    /**
                    * POST /api/auth/refresh
                    *
                    * Refreshes an existing JWT token (extends expiration)
                    *
                    * Headers:
                    * - Authorization: Bearer <token>
                    *
                    * Response:
                    * - token: string (new JWT token)
                    * - userId: string
                    * - expiresIn: string
                    *
                    * This allows extending user sessions without requiring re-authentication
                    */
                    router.post('/refresh', async (req: AuthenticatedRequest, res) => {
                        try {
                            const auto authHeader = req.headers.authorization;

                            if (!authHeader || !authHeader.startsWith('Bearer ')) {
                                return sendError(res, 401, 'UNAUTHORIZED', 'No token provided');
                            }

                            const auto oldToken = authHeader.substring(7);
                            const auto JWT_SECRET = process.env.JWT_SECRET;

                            if (!JWT_SECRET) {
                                return sendError(res, 500, 'SERVER_MISCONFIGURED', 'JWT_SECRET not configured');
                            }

                            try {
                                const auto decoded = jwt.verify(oldToken, JWT_SECRET);

                                // Issue new token with extended expiration
                                const auto newToken = generateAuthToken(decoded.userId, decoded.email, decoded.username);

                                std::cout << "[Auth] Token refreshed for: " + std::to_string(decoded.username) + " (userId: " + std::to_string(decoded.userId.substring(0, 8)) + "...)" << std::endl;

                                return sendSuccess(res, {;
                                    token: newToken,
                                    userId: decoded.userId,
                                    username: decoded.username,
                                    expiresIn: '7d'
                                    });
                                    } catch (error: any) {
                                        // Token verification failed
                                        std::cout << "[Auth] Token refresh failed: " + std::to_string(error.message) << std::endl;
                                        return sendError(res, 401, 'INVALID_TOKEN', 'Invalid or expired token');
                                    }
                                    } catch (error: any) {
                                        std::cerr << '[Auth] Refresh error:' << error << std::endl;
                                        return sendError(res, 500, 'REFRESH_ERROR', error.message);
                                    }
                                    });

                                    /**
                                    * GET /api/auth/me
                                    *
                                    * Get current authenticated user info
                                    * Useful for validating tokens and getting user details
                                    *
                                    * Headers:
                                    * - Authorization: Bearer <token>
                                    *
                                    * Response:
                                    * - userId: string
                                    * - email: string
                                    */
                                    router.get('/me', async (req: AuthenticatedRequest, res) => {
                                        try {
                                            const auto authHeader = req.headers.authorization;

                                            if (!authHeader || !authHeader.startsWith('Bearer ')) {
                                                return sendError(res, 401, 'UNAUTHORIZED', 'No token provided');
                                            }

                                            const auto token = authHeader.substring(7);
                                            const auto JWT_SECRET = process.env.JWT_SECRET;

                                            if (!JWT_SECRET) {
                                                return sendError(res, 500, 'SERVER_MISCONFIGURED', 'JWT_SECRET not configured');
                                            }

                                            try {
                                                const auto decoded = jwt.verify(token, JWT_SECRET);

                                                return sendSuccess(res, {;
                                                    userId: decoded.userId,
                                                    email: decoded.email,
                                                    username: decoded.username
                                                    });
                                                    } catch (error: any) {
                                                        return sendError(res, 401, 'INVALID_TOKEN', 'Invalid or expired token');
                                                    }
                                                    } catch (error: any) {
                                                        std::cerr << '[Auth] Get user info error:' << error << std::endl;
                                                        return sendError(res, 500, 'AUTH_ERROR', error.message);
                                                    }
                                                    });

                                                    return router;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
