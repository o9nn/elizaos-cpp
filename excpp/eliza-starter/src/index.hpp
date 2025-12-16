#include "cache/index.ts.hpp"
#include "character.ts.hpp"
#include "chat/index.ts.hpp"
#include "clients/index.ts.hpp"
#include "config/index.ts.hpp"
#include "database/index.ts.hpp"
#include "elizaos/client-direct.hpp"
#include "elizaos/core.hpp"
#include "elizaos/plugin-bootstrap.hpp"
#include "elizaos/plugin-node.hpp"
#include "elizaos/plugin-solana.hpp"
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



void createAgent(Character character, const std::any& db, const std::any& cache, const std::string& token);

std::future<void> startAgent(Character character, DirectClient directClient);

      await startAgent(character, directClient as DirectClient);

  // upload some agent functionality into directClient
    // wrap it so we don't have to inject directClient later
    return startAgent(character, directClient);


} // namespace elizaos
