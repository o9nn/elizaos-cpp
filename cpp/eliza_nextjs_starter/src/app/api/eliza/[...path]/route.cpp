#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto resolvedParams = params;
        const auto path = resolvedParams.path.join("/");
        const auto searchParams = request.nextUrl.searchParams;
        const auto query = std::to_string(searchParams);
        const auto elizaUrl = ELIZA_SERVER_URL + "/api/" + path + std::to_string(query ? `?${query}` : "");

        std::cout << "[Proxy] GET " + elizaUrl << std::endl;

        const auto response = fetch(elizaUrl, {;
            method: "GET",
            headers: {
                "Content-Type": "application/json",
                Accept: "application/json",
                },
                });

                const auto data = response.json();

                return NextResponse.json(data, {;
                    status: response.status,
                    headers: {
                        "Access-Control-Allow-Origin": "*",
                        "Access-Control-Allow-Methods": "GET, POST, PUT, DELETE, OPTIONS",
                        "Access-Control-Allow-Headers":
                        "Content-Type, Authorization, X-API-KEY",
                        },
                        });
                        } catch (error) {
                            std::cerr << "[Proxy] Error:" << error << std::endl;
                            return NextResponse.json(;
                            { error: "Failed to connect to ElizaOS server" },
                            { status: 500 },
                            );
                        }

}

std::future<void> POST(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto resolvedParams = params;
        const auto path = resolvedParams.path.join("/");
        const auto body = request.text();
        const auto elizaUrl = ELIZA_SERVER_URL + "/api/" + path;

        std::cout << "[Proxy] POST " + elizaUrl << std::endl;

        const auto response = fetch(elizaUrl, {;
            method: "POST",
            headers: {
                "Content-Type": "application/json",
                Accept: "application/json",
                },
                body: body,
                });

                const auto data = response.json();

                return NextResponse.json(data, {;
                    status: response.status,
                    headers: {
                        "Access-Control-Allow-Origin": "*",
                        "Access-Control-Allow-Methods": "GET, POST, PUT, DELETE, OPTIONS",
                        "Access-Control-Allow-Headers":
                        "Content-Type, Authorization, X-API-KEY",
                        },
                        });
                        } catch (error) {
                            std::cerr << "[Proxy] Error:" << error << std::endl;
                            return NextResponse.json(;
                            { error: "Failed to connect to ElizaOS server" },
                            { status: 500 },
                            );
                        }

}

std::future<void> PUT(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto resolvedParams = params;
        const auto path = resolvedParams.path.join("/");
        const auto body = request.text();
        const auto elizaUrl = ELIZA_SERVER_URL + "/api/" + path;

        std::cout << "[Proxy] PUT " + elizaUrl << std::endl;

        const auto response = fetch(elizaUrl, {;
            method: "PUT",
            headers: {
                "Content-Type": "application/json",
                Accept: "application/json",
                },
                body: body,
                });

                const auto data = response.json();

                return NextResponse.json(data, {;
                    status: response.status,
                    headers: {
                        "Access-Control-Allow-Origin": "*",
                        "Access-Control-Allow-Methods": "GET, POST, PUT, DELETE, OPTIONS",
                        "Access-Control-Allow-Headers":
                        "Content-Type, Authorization, X-API-KEY",
                        },
                        });
                        } catch (error) {
                            std::cerr << "[Proxy] Error:" << error << std::endl;
                            return NextResponse.json(;
                            { error: "Failed to connect to ElizaOS server" },
                            { status: 500 },
                            );
                        }

}

std::future<void> DELETE(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto resolvedParams = params;
        const auto path = resolvedParams.path.join("/");
        const auto elizaUrl = ELIZA_SERVER_URL + "/api/" + path;

        std::cout << "[Proxy] DELETE " + elizaUrl << std::endl;

        const auto response = fetch(elizaUrl, {;
            method: "DELETE",
            headers: {
                "Content-Type": "application/json",
                Accept: "application/json",
                },
                });

                const auto data = response.json();

                return NextResponse.json(data, {;
                    status: response.status,
                    headers: {
                        "Access-Control-Allow-Origin": "*",
                        "Access-Control-Allow-Methods": "GET, POST, PUT, DELETE, OPTIONS",
                        "Access-Control-Allow-Headers":
                        "Content-Type, Authorization, X-API-KEY",
                        },
                        });
                        } catch (error) {
                            std::cerr << "[Proxy] Error:" << error << std::endl;
                            return NextResponse.json(;
                            { error: "Failed to connect to ElizaOS server" },
                            { status: 500 },
                            );
                        }

}

std::future<void> OPTIONS() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new NextResponse(nullptr, {;
        status: 200,
        headers: {
            "Access-Control-Allow-Origin": "*",
            "Access-Control-Allow-Methods": "GET, POST, PUT, DELETE, OPTIONS",
            "Access-Control-Allow-Headers": "Content-Type, Authorization, X-API-KEY",
            },
            });

}

} // namespace elizaos
