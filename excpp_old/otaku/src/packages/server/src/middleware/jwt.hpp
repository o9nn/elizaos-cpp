#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct AuthTokenPayload {
    std::string userId;
    std::string email;
    std::string username;
    std::optional<bool> isAdmin;
    double iat;
    double exp;
};

/**
 * Generate JWT authentication token
 * Uses CDP's userId directly (no generation or salting needed)
 */
std::string generateAuthToken(const std::string& userId, const std::string& email, const std::string& username, std::optional<bool> isAdmin);

/**
 * Middleware to verify JWT token and extract user info
 * Requires authentication - returns 401 if no valid token
 */
void requireAuth(AuthenticatedRequest req, Response res, NextFunction next);

/**
 * Optional middleware for endpoints that work with or without auth
 * If token is provided and valid, sets userId and userEmail
 * If token is invalid or missing, continues without setting them
 */
void optionalAuth(AuthenticatedRequest req, NextFunction next);

/**
 * Middleware to accept either JWT Bearer token or X-API-KEY.
 * - If JWT is valid, sets user fields on request.
 * - If X-API-KEY matches ELIZA_SERVER_AUTH_TOKEN, marks request as server-authenticated.
 * - Otherwise, returns 401.
 */
void requireAuthOrApiKey(AuthenticatedRequest req, Response res, NextFunction next);

/**
 * Middleware to require admin access
 * Must be used after requireAuth middleware
 */
void requireAdmin(AuthenticatedRequest req, Response res, NextFunction next);


} // namespace elizaos
