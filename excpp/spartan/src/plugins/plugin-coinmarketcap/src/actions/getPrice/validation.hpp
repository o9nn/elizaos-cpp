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
import type { GetPriceContent } from "./types";

const GetPriceSchema = z.object({
    symbol: z.string(),
    currency: z.string().default("USD"),
});



} // namespace elizaos
