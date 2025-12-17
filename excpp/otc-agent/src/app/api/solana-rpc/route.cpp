#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> POST(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto heliusKey = process.env.HELIUS_API_KEY;

    if (!heliusKey) {
        return NextResponse.json(;
        { error: "Solana RPC not configured" },
        { status: 500 },
        );
    }

    try {
        const auto body = request.json();

        const auto response = fetch(;
        "https://mainnet.helius-rpc.com/?api-key=" + heliusKey
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
                },
                body: /* JSON.stringify */ std::string(body),
                },
                );

                const auto data = response.json();
                return NextResponse.json(data);
                } catch (error) {
                    std::cerr << "[Solana RPC Proxy] Error:" << error << std::endl;
                    return NextResponse.json({ error: "RPC request failed" }, { status: 500 });
                }

}

} // namespace elizaos
