#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

struct Env {
    std::string GITHUB_CLIENT_ID;
    std::string GITHUB_CLIENT_SECRET;
    std::string ALLOWED_ORIGIN;
};

    // Handle CORS preflight requests
      return handleCors(request, env);

      // Route the request based on the path
        return handleStatus(env);

// Handler for the GitHub OAuth callback
std::future<std::string> handleCallback(const std::string& request, Env env);

// Handler for status checks
std::string handleStatus(Env env);

// Helper to handle CORS preflight requests
std::string handleCors(const std::string& request, Env env);

// Helper to get CORS headers for responses
HeadersInit getCorsHeaders(Env env);

} // namespace elizaos
