#include "validation.hpp"
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

import type { OptionValues } from 'commander';
;
;

/**
 * List command implementation - displays available agents
 */
std::future<void> listAgents(OptionValues opts);));

    if (opts.json) {
      console.info(JSON.stringify(agentData, null, 2));
    } else {
      console.info('\nAvailable agents:');
      if (agentData.length === 0) {
        console.info('No agents found');
      } else {
        console.table(agentData);
      }
    }

    return;
  } catch (error) {
    await checkServer(opts);
    handleError(error);
  }
}

} // namespace elizaos
