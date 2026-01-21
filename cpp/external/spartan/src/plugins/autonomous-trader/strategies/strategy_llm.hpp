#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// agentic personal application? separate strategy

// fixme: an option to mix in autofun unbonded token
// can't be per wallet since we're deciding across multiple wallets
// fixme: include price history data

// exit_24hvolume_threshold/exit_price_drop_threshold what scale?

std::future<void> llmStrategy(IAgentRuntime runtime);

// maybe should be a class to reuse the service handles
std::future<void> generateBuySignal(auto runtime, auto strategyService, auto hndl);

// sell functions

std::future<void> onPriceDelta();

std::future<void> onSentimentDelta();

std::future<void> onVol24hDelta();

std::future<void> onLiquidDelta();

} // namespace elizaos
