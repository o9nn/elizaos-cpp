#include ".birdeye.hpp"
#include ".utils.hpp"
#include "elizaos/core.hpp"
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



// "strict" requires a $ prefix and will match $SOL, $ai16z, $BTC, etc.
// "loose" will match $SOL, SOL, SOLANA, etc. and does not require a $ prefix but may interpret any other acronyms as symbols to search for

            // get all symbols from the message that match (i.e. $SOL, $ETH, $BTC, etc.). If you want to match more loosely, use "loose" instead of "strict" and it will match $SOL, SOL, SOLANA, etc.

            // for each symbol, do a search in Birdeye. This will return a list of token results that may be amatch to the token symbol.

            // get filter the results to only include the token results and then filter the results to only include the ones that match the symbol


} // namespace elizaos
