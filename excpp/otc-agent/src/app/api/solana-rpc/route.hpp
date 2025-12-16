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

;

/**
 * Proxy for Solana RPC calls to hide the API key from the client
 */
async ,
      { status: 500 },
    );
  }

  try {
    const body = await request.json();

    const response = await fetch(
      `https://mainnet.helius-rpc.com/?api-key=${heliusKey}`,
      {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify(body),
      },
    );

    const data = await response.json();
    return NextResponse.json(data);
  } catch (error) {
    console.error("[Solana RPC Proxy] Error:", error);
    return NextResponse.json({ error: "RPC request failed" }, { status: 500 });
  }
}

} // namespace elizaos
