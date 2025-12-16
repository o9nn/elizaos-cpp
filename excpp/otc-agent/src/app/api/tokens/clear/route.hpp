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
;

/**
 * Clear all tokens from the database (dev utility)
 * POST /api/tokens/clear
 */
async ,
      { status: 403 },
    );
  }

  try {
    const runtime = await agentRuntime.getRuntime();

    // Get all token IDs
    const allTokenIds = (await runtime.getCache<string[]>("all_tokens")) ?? [];

    // Delete each token
    for (const tokenId of allTokenIds) {
      await runtime.setCache(`token:${tokenId}`, null);
    }

    // Clear the token list
    await runtime.setCache("all_tokens", []);

    return NextResponse.json({
      success: true,
      message: `Cleared ${allTokenIds.length} tokens`,
      clearedTokens: allTokenIds,
    });
  } catch (error) {
    console.error("[Clear Tokens] Error:", error);
    return NextResponse.json(
      { error: "Failed to clear tokens" },
      { status: 500 },
    );
  }
}

} // namespace elizaos
