#include ".constants.hpp"
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



  
/**
 * A provider that lists all available emotes with descriptions.
 * @type {Provider}
 */
const hyperfyEmoteProvider: Provider = {
    name: 'HYPERFY_EMOTE_LIST',
    description: 'Lists all available emotes and their descriptions',
    get: async (_runtime: IAgentRuntime, _message: Memory) => {
      const animationListText = EMOTES_LIST.map(
        (e) => `- **${e.name}**: ${e.description}`
      ).join('\n');
      const animationText = `## Available Animations\n${animationListText}`;
      
      return {
        data: {
          emotes: EMOTES_LIST,
        },
        values: {
          hyperfyAnimations: animationText,
        },
        text: animationText,
      };
    },
};
  
} // namespace elizaos
