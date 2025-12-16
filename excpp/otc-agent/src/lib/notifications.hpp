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

std::future<bool> sendWelcomeNotification(double fid);,
      body: JSON.stringify({
        fid,
        title: "Welcome to Eliza OTC Desk",
        body: "Start trading with AI-powered negotiation on Base, BSC, and Solana",
      }),
    });

    const result = await response.json();
    return result.state === "success";
  } catch (error) {
    console.error("Failed to send welcome notification:", error);
    return false;
  }
}

} // namespace elizaos
