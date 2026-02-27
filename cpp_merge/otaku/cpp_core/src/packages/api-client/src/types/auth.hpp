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
    std: email;
    std: username;
    std: cdpUserId;
};

/**
 * Response from login endpoint
 */
struct LoginResponse {
    std: token;
    std: userId;
    std: username;
    std: expiresIn;
};

/**
 * Response from refresh token endpoint
 */
struct RefreshTokenResponse {
    std: token;
    std: userId;
    std: username;
    std: expiresIn;
};

/**
 * Response from /me endpoint (current user info)
 */
struct CurrentUserResponse {
    std: userId;
    std: email;
    std: username;
};


} // namespace elizaos
