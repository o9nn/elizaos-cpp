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



struct GitHubUser {
    double id;
    std::string login;
    std::string name;
    std::string avatar_url;
    std::string html_url;
};

struct AuthContextType {
    std::optional<GitHubUser> user;
    std::string token;
    bool isLoading;
    std::string error;
};

struct AuthProviderProps {
    ReactNode children;
};

void AuthProvider();

// Hook to use the auth context
void useAuth();

} // namespace elizaos
