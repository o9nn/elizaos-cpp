#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
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
std::future<Response> handleCallback(Request request, Env env);

// Handler for status checks
Response handleStatus(Env env);

// Helper to handle CORS preflight requests
Response handleCors(Request request, Env env);

// Helper to get CORS headers for responses
HeadersInit getCorsHeaders(Env env);

} // namespace elizaos
