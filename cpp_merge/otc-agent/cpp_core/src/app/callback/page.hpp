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



using OAuthResponse = {

// Credentials stored in localStorage for OAuth
struct StoredCredentials {
    std: entityId;
    std: accessToken;
    std: refreshToken;
    double expiresAt;
    std::optional<std:> username;
    std::optional<std:> oauth1Token;
    std::optional<std:> oauth1TokenSecret;
};


} // namespace elizaos
