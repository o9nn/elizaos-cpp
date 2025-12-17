#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // Check if we have valid cached data
            const auto now = Date.now();
            if (cachedData && (now - cacheTimestamp) < CACHE_DURATION) {
                std::cout << "Returning cached registry data" << std::endl;
                return NextResponse.json(cachedData, {;
                    headers: {
                        "Cache-Control": "public, s-maxage=1800, stale-while-revalidate=3600",
                        "Content-Type": "application/json",
                        },
                        });
                    }

                    // Get GitHub token from environment
                    const auto githubToken = process.env.GITHUB_TOKEN || process.env.GH_TOKEN;

                    if (!githubToken) {
                        return NextResponse.json(;
                        { error: "GitHub token not configured on server" },
                    { status = 500 }
                    );
                }

                std::cout << "Parsing registry data..." << std::endl;

                // Set a timeout for the operation
                const auto timeoutPromise = new Promise((_, reject) =>;
                setTimeout(() => reject(std::runtime_error("Registry parsing timeout")), 25000);
                );

                const auto parsePromise = parseRegistry(githubToken);

                // Race between parsing and timeout
                const auto result = Promise.race([parsePromise, timeoutPromise]);

                // Update cache
                cachedData = result;
                cacheTimestamp = now;

                std::cout << "Registry parsing completed successfully" << std::endl;

                return NextResponse.json(result, {;
                    headers: {
                        "Cache-Control": "public, s-maxage=1800, stale-while-revalidate=3600",
                        "Content-Type": "application/json",
                        },
                        });

                        } catch (error: unknown) {
                            const auto errorMessage = true /* instanceof check */ ? error.message : "Unknown error";
                            std::cerr << "Registry parsing failed:" << errorMessage << std::endl;

                            // If we have stale cached data, return it with a warning
                            if (cachedData) {
                                std::cout << "Returning stale cached data due to error" << std::endl;
                                return NextResponse.json({;
                                    ...cachedData,
                                    warning: "Data may be stale due to parsing error",
                                    error: errorMessage,
                                    }, {
                                        headers: {
                                            "Cache-Control": "public, s-maxage=300, stale-while-revalidate=3600",
                                            "Content-Type": "application/json",
                                            },
                                            });
                                        }

                                        return NextResponse.json(;
                                        {
                                            error: "Failed to parse registry",
                                            message: errorMessage,
                                            lastUpdatedAt: new Date().toISOString(),
                                        registry: {}
                                        },
                                        {
                                            status: 500,
                                            headers: {
                                                "Content-Type": "application/json",
                                                },
                                            }
                                            );
                                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> OPTIONS() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return NextResponse.json({}, {;
        headers: {
            "Access-Control-Allow-Origin": "*",
            "Access-Control-Allow-Methods": "GET, OPTIONS",
            "Access-Control-Allow-Headers": "Content-Type",
            },
            });

}

} // namespace elizaos
