#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Request body for login endpoint
 */
struct LoginRequest {
    std::string email;
    std::string username;
    std::string cdpUserId;
};

/**
 * Response from login endpoint
 */
struct LoginResponse {
    std::string token;
    std::string userId;
    std::string username;
    std::string expiresIn;
};

/**
 * Response from refresh token endpoint
 */
struct RefreshTokenResponse {
    std::string token;
    std::string userId;
    std::string username;
    std::string expiresIn;
};

/**
 * Response from /me endpoint (current user info)
 */
struct CurrentUserResponse {
    std::string userId;
    std::string email;
    std::string username;
};


} // namespace elizaos
