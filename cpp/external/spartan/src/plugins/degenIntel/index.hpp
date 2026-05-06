#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "apis.hpp"
#include "elizaos/core.hpp"
#include "providers/birdeyeTrending.hpp"
#include "providers/birdeyeWallet.hpp"
#include "providers/cmcMarket.hpp"
#include "providers/sentiment.hpp"
#include "services/srv_chain.hpp"
#include "services/srv_dataprovider.hpp"
#include "services/srv_strategy.hpp"
#include "tasks.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// INTEL_SYNC_WALLET provider? or solana handles this?

// Services

// create a new plugin

    // check for cmc key, if have then register provider

    // check for birdeeye key, if have then register provider

    // twitter for sentiment


} // namespace elizaos
