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



// Extend the built-in session types
  struct Session {
    { user;
    std::string id;
    { connections;
    std::string name;
    std::string username;
    std::string image;
    std::optional<std::string> accessToken;
    std::optional<double> expirationTime;
    std::optional<std::string> refreshToken;
    std::optional<double> refreshTokenExpirationTime;
    std::optional<bool> hasLinkedSolana;
    std::optional<std::string> provider;
    std::optional<std::string> accessToken;
    std::optional<std::string> refreshToken;
};

  struct User {
    std::optional<std::string> username;
    std::optional<std::string> provider;
};

bool verifyTelegramAuth(const std::any& data);

default NextAuth(authOptions);

} // namespace elizaos
