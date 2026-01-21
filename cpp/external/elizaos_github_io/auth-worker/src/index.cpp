#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::string> handleCallback(const std::string& request, Env env) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto url = new URL(request.url);
        const auto code = url.searchParams.get("code");

        if (!code) {
            return new Response(;
            /* JSON.stringify */ std::string({ error: "Missing authorization code" }),
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
                    const auto tokenResponse = fetch(;
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
                                    throw std::runtime_error(`GitHub token exchange failed: ${tokenResponse.status}`);
                                }

                                const auto tokenData = (tokenResponse.json()) as {;
                                    access_token: string;
                                    token_type: string;
                                    scope: string;
                                    error?: string;
                                    error_description?: string;
                                    };
                                    std::cout << "Token data:" << tokenData << std::endl;
                                    // Check if we got an error response from GitHub
                                    if (tokenData.error) {
                                        return new Response(;
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
                                                if (!tokenData.scope.includes("read:user")) {
                                                    return new Response(;
                                                    JSON.stringify({
                                                        error: "Insufficient permissions. The "read:user" scope is required.",
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
                                                            const auto expiresInMilliseconds = 3 * 60 * 60 * 1000; // 3 hours;
                                                            const auto expiresAt = Date.now() + expiresInMilliseconds;

                                                            return new Response(;
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
                                                                            std::cerr << "Error exchanging code for token:" << error << std::endl;
                                                                            return new Response(;
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

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::string handleStatus(Env env) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new Response(/* JSON.stringify */ std::string({ status: "ok" }), {;
        status: 200,
        headers: {
            "Content-Type": "application/json",
            ...getCorsHeaders(env),
            },
            });

}

std::string handleCors(const std::string& request, Env env) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new Response(nullptr, {;
        status: 204,
        headers: {
            ...getCorsHeaders(env),
            "Access-Control-Allow-Methods": "GET, POST, OPTIONS",
            "Access-Control-Allow-Headers": "Content-Type, Authorization",
            "Access-Control-Max-Age": "86400",
            },
            });

}

HeadersInit getCorsHeaders(Env env) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        "Access-Control-Allow-Origin": env.ALLOWED_ORIGIN,
        "Access-Control-Allow-Credentials": "true",
        };

}

} // namespace elizaos
