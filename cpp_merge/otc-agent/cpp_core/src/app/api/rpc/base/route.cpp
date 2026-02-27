#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> POST(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!ALCHEMY_API_KEY) {
        // Fall back to public RPC if no Alchemy key
        const auto publicRpc = "https://mainnet.base.org";
        try {
            const auto body = request.json();
            const auto response = fetch(publicRpc, {;
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: /* JSON.stringify */ std::string(body),
                });
                const auto data = response.json();
                return NextResponse.json(data);
                } catch (error) {
                    std::cerr << "[RPC Proxy] Public RPC error:" << error << std::endl;
                    return NextResponse.json(;
                    { error: "RPC request failed" },
                    { status: 502 },
                    );
                }
            }

            try {
                const auto body = request.json();

                const auto response = fetch(ALCHEMY_BASE_URL, {;
                    method: "POST",
                    headers: {
                        "Content-Type": "application/json",
                        },
                        body: /* JSON.stringify */ std::string(body),
                        });

                        if (!response.ok) {
                            console.error(
                            "[RPC Proxy] Alchemy error:",
                            response.status,
                            response.statusText,
                            );
                            return NextResponse.json(;
                            { error: "RPC request failed" },
                            { status: response.status },
                            );
                        }

                        const auto data = response.json();
                        return NextResponse.json(data);
                        } catch (error) {
                            std::cerr << "[RPC Proxy] Error:" << error << std::endl;
                            return NextResponse.json({ error: "RPC proxy error" }, { status: 500 });
                        }

}

} // namespace elizaos
