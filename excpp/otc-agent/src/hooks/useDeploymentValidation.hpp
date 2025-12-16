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

"use client";

;
;
;

 (OTC Contract)`);
      }
      if (isSolanaChain(chain)) {
        if (!config.contracts.otc) missing.push(`${chain} (Desk Address)`);
      }
    }

    if (missing.length > 0) {
      console.error(
        `❌ CRITICAL: Missing deployment configuration for: ${missing.join(", ")}. ` +
          `Please ensure deployment JSONs are present in src/config/deployments/ or env vars are set.`,
      );
      // Optional: Display a toast or blocking modal in UI
    }
  }, []);
}

} // namespace elizaos
