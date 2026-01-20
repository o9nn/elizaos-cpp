#include "notifications.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<bool> sendWelcomeNotification(double fid) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto response = fetch("/api/notifications/send", {;
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({
                fid,
                title: "Welcome to Eliza OTC Desk",
                body: "Start trading with AI-powered negotiation on Base, BSC, and Solana",
                }),
                });

                const auto result = response.json();
                return result.state == "success";
                } catch (error) {
                    std::cerr << "Failed to send welcome notification:" << error << std::endl;
                    return false;
                }

}

} // namespace elizaos
