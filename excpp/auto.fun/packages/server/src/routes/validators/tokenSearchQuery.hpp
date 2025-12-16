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
const SearchTokenBodySchema = z.object({
   mint: z
      .string()
      .min(32, { message: "Mint address too short" })
      .max(44, { message: "Mint address too long" })
      .refine((val) => {
         try {
            new PublicKey(val);
            return true;
         } catch {
            return false;
         }
      }, { message: "Invalid mint address format" }),
   requestor: z.string().min(1, { message: "Missing requestor" })
});

using SearchTokenInput = z.infer<typeof SearchTokenBodySchema>;



} // namespace elizaos
