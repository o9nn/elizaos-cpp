#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".db.hpp"
#include ".env.hpp"
#include ".redis.hpp"
#include ".routes/user.hpp"
#include ".util.hpp"
#include "session.hpp"

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
};

using AppContext = Context<{

struct AuthTokenData {
    std::string publicKey;
    std::string tokenId;
    double timestamp;
    std::optional<std::vector<std::string>> privileges;
    std::optional<double> expiresAt;
};

          // jwt is not used now by the backend at all

std::future<std::string> createJwtToken(const std::string& publicKey);


} // namespace elizaos
