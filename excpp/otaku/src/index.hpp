#include "character.hpp"
#include "elizaos/core.hpp"
#include "elizaos/plugin-analytics.hpp"
#include "elizaos/plugin-mcp.hpp"
#include "elizaos/plugin-openai.hpp"
#include "elizaos/plugin-openrouter.hpp"
#include "elizaos/plugin-sql.hpp"
#include "plugins/plugin-bootstrap/src/index.ts.hpp"
#include "plugins/plugin-cdp/index.ts.hpp"
#include "plugins/plugin-clanker/src/index.ts.hpp"
#include "plugins/plugin-coingecko/src/index.ts.hpp"
#include "plugins/plugin-defillama/src/index.ts.hpp"
#include "plugins/plugin-etherscan/src/index.ts.hpp"
#include "plugins/plugin-gamification/src/index.ts.hpp"
#include "plugins/plugin-morpho/src/index.ts.hpp"
#include "plugins/plugin-relay/src/index.ts.hpp"
#include "plugins/plugin-web-search/src/index.ts.hpp"
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
;
;
;
;
;
;
;
;
;
;

;
;
;
;
;

const initCharacter = ({ runtime }: { runtime: IAgentRuntime }) => {
  logger.info('Initializing character');
  logger.info({ name: character.name }, 'Character loaded:');
};

const projectAgent: ProjectAgent = {
  character,
  init: async (runtime: IAgentRuntime) => await initCharacter({ runtime }),
  // Import actual plugin modules instead of using string names
  plugins: [
    sqlPlugin, 
    bootstrapPlugin, 
    openrouterPlugin,
    openaiPlugin, 
    cdpPlugin, 
    coingeckoPlugin, 
    webSearchPlugin,
    defiLlamaPlugin,
    relayPlugin,
    etherscanPlugin,
    mcpPlugin,
    analyticsPlugin,
    clankerPlugin,
    morphoPlugin,
    gamificationPlugin,
    // x402DiscoveryPlugin,
  ],
};

const project: Project = {
  agents: [projectAgent],
};

{ character } from './character';

default project;


} // namespace elizaos
