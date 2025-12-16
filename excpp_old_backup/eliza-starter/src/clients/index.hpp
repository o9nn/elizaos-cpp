#include "elizaos/client-auto.hpp"
#include "elizaos/client-discord.hpp"
#include "elizaos/client-telegram.hpp"
#include "elizaos/client-twitter.hpp"
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



std::future<void> initializeClients(Character character, IAgentRuntime runtime);

} // namespace elizaos
