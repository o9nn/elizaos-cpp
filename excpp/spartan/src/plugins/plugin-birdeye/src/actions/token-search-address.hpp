#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".birdeye.hpp"
#include ".utils.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using TokenAddressSearchResult = {

            // runtime.cacheManager

            // get all contract addresses from the message

            // for each symbol, do a search in Birdeye. This will return a list of token results that may be amatch to the token symbol.
                    // address detection can't distinguish between evm chains, so we currently only do address search on ETH for EVM addresses. Future support will be added for other chains if the user requests it.

// take all the details of the results and present to the user


} // namespace elizaos
