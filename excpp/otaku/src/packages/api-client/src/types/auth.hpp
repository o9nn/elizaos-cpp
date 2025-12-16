#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Request body for login endpoint
 */
struct LoginRequest {
    std::string email;
    string; // User's display name from CDP username;
    string; // CDP's user identifier (UUID) cdpUserId;
};


/**
 * Response from login endpoint
 */
struct LoginResponse {
    string; // JWT authentication token token;
    string; // Secure user ID generated from email userId;
    string; // User's display name username;
    string; // Token expiration time (e.g., "7d") expiresIn;
};


/**
 * Response from refresh token endpoint
 */
struct RefreshTokenResponse {
    string; // New JWT authentication token token;
    std::string userId;
    string; // User's display name username;
    std::string expiresIn;
};


/**
 * Response from /me endpoint (current user info)
 */
struct CurrentUserResponse {
    std::string userId;
    std::string email;
    string; // User's display name username;
};



} // namespace elizaos
