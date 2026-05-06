#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using OAuthResponse = {

// Credentials stored in localStorage for OAuth
struct StoredCredentials {
    std::string entityId;
    std::string accessToken;
    std::string refreshToken;
    double expiresAt;
    std::optional<std::string> username;
    std::optional<std::string> oauth1Token;
    std::optional<std::string> oauth1TokenSecret;
};


} // namespace elizaos
