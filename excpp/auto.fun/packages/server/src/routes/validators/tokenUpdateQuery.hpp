#include "global.hpp"
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

const updateTokenBodySchema = z.object({
   website: z.string().url().optional(),
   twitter: z.string().url().optional(),
   telegram: z.string().url().optional(),
   discord: z.string().url().optional(),
   farcaster: z.string().optional(),
});


using UpdateTokenBody = z.infer<typeof updateTokenBodySchema>;



): { mint: string; body: UpdateTokenBody; userId: string } {
   const mint = parseSolanaAddress(raw.mint, "mint address");

   const body = updateTokenBodySchema.parse(raw.body);

   if (
      typeof raw.user !== "object" ||
      raw.user === null ||
      typeof (raw.user as any).id !== "string"
   ) {
      throw new Error("Unauthorized: missing or invalid user");
   }
   const userId = (raw.user as any).id;

   return { mint, body, userId };
}

} // namespace elizaos
