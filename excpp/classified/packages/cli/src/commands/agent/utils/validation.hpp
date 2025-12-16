#include "..shared.hpp"
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
import type { AgentBasic } from '../../shared';
;

// Zod schemas for validation
const AgentBasicSchema = z
  .object({
    id: z.string(),
    name: z.string(),
    status: z.string().optional(),
  })
  .passthrough(); // Allow additional properties

const AgentsListResponseSchema = z.object({
  success: z.boolean(),
  data: z
    .object({
      agents: z.array(AgentBasicSchema),
    })
    .optional(),
});

/**
 * Asynchronously fetches a list of basic agent information from the server.
 */
std::future<std::vector<AgentBasic>> getAgents(OptionValues opts);`);
  }
  const rawData = await response.json();
  const validatedData = AgentsListResponseSchema.parse(rawData);
  return (validatedData.data?.agents || []) as AgentBasic[];
}

/**
 * Resolves the ID of an agent based on the provided name, ID, or index.
 */
std::future<std::string> resolveAgentId(const std::string& idOrNameOrIndex, OptionValues opts);

  // Try to find agent by ID
  const agentById = agents.find((agent) => agent.id === idOrNameOrIndex);

  if (agentById) {
    return agentById.id;
  }

  // Try to find agent by index
  if (!Number.isNaN(Number(idOrNameOrIndex))) {
    const indexAgent = agents[Number(idOrNameOrIndex)];
    if (indexAgent) {
      return indexAgent.id;
    }
  }

  // If no agent is found, throw a specific error type that we can catch
  throw new Error(`AGENT_NOT_FOUND:${idOrNameOrIndex}`);
}

} // namespace elizaos
