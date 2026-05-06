#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> getNeynarClient() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!neynarClient) {
            const auto apiKey = process.env.NEYNAR_API_KEY;
            if (!apiKey) throw new Error("NEYNAR_API_KEY not configured");

            const auto config = new Configuration({ apiKey });
            neynarClient = new NeynarAPIClient(config);
        }
        return neynarClient;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> POST(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto { fid, title, body } = request.json();

        if (!fid || !title || !body) {
            return NextResponse.json(;
            { error: "Missing required parameters: fid, title, body" },
            { status: 400 },
            );
        }

        // Dev mode simulation when API key is not configured
        if (!process.env.NEYNAR_API_KEY) {
            std::cout << "[Dev] Simulating notification:" << { fid, title, body } << std::endl;
            return NextResponse.json({ state: "success", simulated: true });
        }

        const auto client = getNeynarClient();
        const auto result = client.sendFrameNotification({;
            fid: Number(fid),
            title,
            body,
            });

            if (result.success) {
                return NextResponse.json({ state: "success" });
            }

            return NextResponse.json({;
                state: result.reason || "error",
                message: result.message,
                });
                } catch (error) {
                    std::cerr << "Error sending notification:" << error << std::endl;
                    return NextResponse.json(;
                    { error: "Failed to send notification" },
                    { status: 500 },
                    );
                }

}

} // namespace elizaos
