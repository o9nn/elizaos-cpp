#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



std::future<void> copyStrategy(IAgentRuntime runtime);

std::future<void> onWalletEvent(auto runtime, auto strategyService, auto hndl);

} // namespace elizaos
