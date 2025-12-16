#include ".db.hpp"
#include ".env.hpp"
#include ".redis.hpp"
#include ".routes/user.hpp"
#include ".util.hpp"
#include "session.hpp"
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



// Define the AuthTokenData interface here to fix TypeScript errors
struct AuthTokenData {
    std::string publicKey;
    std::string tokenId;
    double timestamp;
    std::optional<std::vector<std::string>> privileges;
    std::optional<double> expiresAt;
};

  struct ContextVariableMap {
    std::optional<{ publicKey: string } | null> user;

using AppContext = Context<{

struct AuthTokenData {
    std::string publicKey;
    std::string tokenId;
    double timestamp;
    std::optional<std::vector<std::string>> privileges;
    std::optional<double> expiresAt;
};

        await ensureUserProfile(address);

          // jwt is not used now by the backend at all

std::future<std::string> createJwtToken(const std::string& publicKey);


} // namespace elizaos
