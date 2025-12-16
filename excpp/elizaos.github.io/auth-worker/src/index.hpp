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

struct Env {
    std::string GITHUB_CLIENT_ID;
    std::string GITHUB_CLIENT_SECRET;
    std::string ALLOWED_ORIGIN;
};


const worker = {
  async fetch(request: Request, env: Env, ctx: unknown): Promise<Response> {
    console.log("Request received:", { request, ctx });
    // Handle CORS preflight requests
    if (request.method === "OPTIONS") {
      return handleCors(request, env);
    }

    const url = new URL(request.url);

    try {
      // Route the request based on the path
      if (url.pathname === "/api/auth/callback") {
        return await handleCallback(request, env);
      } else if (url.pathname === "/api/status") {
        return handleStatus(env);
      } else {
        return new Response("Not found", { status: 404 });
      }
    } catch (error) {
      console.error("Error in worker:", error);
      return new Response(JSON.stringify({ error: "Internal server error" }), {
        status: 500,
        headers: {
          "Content-Type": "application/json",
          ...getCorsHeaders(env),
        },
      });
    }
  },
};

default worker;
// Handler for the GitHub OAuth callback
std::future<Response> handleCallback(Request request, Env env);),
      {
        status: 400,
        headers: {
          "Content-Type": "application/json",
          ...getCorsHeaders(env),
        },
      },
    );
  }

  try {
    // Exchange the code for an access token
    const tokenResponse = await fetch(
      "https://github.com/login/oauth/access_token",
      {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          Accept: "application/json",
        },
        body: JSON.stringify({
          client_id: env.GITHUB_CLIENT_ID,
          client_secret: env.GITHUB_CLIENT_SECRET,
          code,
        }),
      },
    );

    if (!tokenResponse.ok) {
      throw new Error(`GitHub token exchange failed: ${tokenResponse.status}`);
    }

    const tokenData = (await tokenResponse.json()) as {
      access_token: string;
      token_type: string;
      scope: string;
      error?: string;
      error_description?: string;
    };
    console.log("Token data:", tokenData);
    // Check if we got an error response from GitHub
    if (tokenData.error) {
      return new Response(
        JSON.stringify({
          error: tokenData.error,
          description: tokenData.error_description,
        }),
        {
          status: 400,
          headers: {
            "Content-Type": "application/json",
            ...getCorsHeaders(env),
          },
        },
      );
    }

    // Check if the token has the required scope
    if (!tokenData.scope?.includes("read:user")) {
      return new Response(
        JSON.stringify({
          error: "Insufficient permissions. The 'read:user' scope is required.",
        }),
        {
          status: 403,
          headers: {
            "Content-Type": "application/json",
            ...getCorsHeaders(env),
          },
        },
      );
    }

    // Return the token data to the client
    const expiresInMilliseconds = 3 * 60 * 60 * 1000; // 3 hours
    const expiresAt = Date.now() + expiresInMilliseconds;

    return new Response(
      JSON.stringify({
        access_token: tokenData.access_token,
        token_type: tokenData.token_type,
        scope: tokenData.scope,
        expires_at: expiresAt,
      }),
      {
        status: 200,
        headers: {
          "Content-Type": "application/json",
          ...getCorsHeaders(env),
        },
      },
    );
  } catch (error) {
    console.error("Error exchanging code for token:", error);
    return new Response(
      JSON.stringify({
        error: "Failed to exchange authorization code for access token",
      }),
      {
        status: 500,
        headers: {
          "Content-Type": "application/json",
          ...getCorsHeaders(env),
        },
      },
    );
  }
}

// Handler for status checks
), {
    status: 200,
    headers: {
      "Content-Type": "application/json",
      ...getCorsHeaders(env),
    },
  });
}

// Helper to handle CORS preflight requests
,
  });
}

// Helper to get CORS headers for responses
;
}

} // namespace elizaos
