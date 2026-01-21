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



struct GitHubUser {
    double id;
    std::string login;
    std::string | null name;
    std::string avatar_url;
    std::string html_url;
};

struct AuthContextType {
    GitHubUser | null user;
    std::string | null token;
    bool isLoading;
    std::string | null error;
    () => void signin;
    () => void signout;
    (code: std::string, state: std::string) => Promise<void> handleAuthCallback;
};

struct AuthProviderProps {
    ReactNode children;
};

void AuthProvider(AuthProviderProps { children });

// Hook to use the auth context
void useAuth();

} // namespace elizaos
