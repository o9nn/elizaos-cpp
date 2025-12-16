#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".lib/base-client.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Service for JWT authentication endpoints
 */
class AuthService {
public:
    std::future<LoginResponse> login(LoginRequest request);
    std::future<RefreshTokenResponse> refreshToken();
    std::future<CurrentUserResponse> getCurrentUser();
};


} // namespace elizaos
