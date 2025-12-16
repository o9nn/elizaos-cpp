#include "..shared.hpp"
#include ".utils.hpp"
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

import type { Agent } from '@elizaos/core';
;
import type { OptionValues } from 'commander';
;
;
;
import type { ApiResponse } from '../../shared';
;
;

/**
 * Safely parse JSON response with error handling
 * @param response - The fetch Response object
 * @returns Parsed JSON data or null if parsing fails
 */
async  catch (error) {
    console.error('Failed to parse response as JSON:', error);
    return null;
  }
}

/**
 * Parse error response and throw appropriate error
 * @param response - The fetch Response object
 * @param defaultMessage - Default error message if JSON parsing fails
 */
std::future<never> handleErrorResponse(Response response, const std::string& defaultMessage);

/**
 * Get command implementation - retrieves and displays agent details
 */
std::future<void> getAgent(OptionValues opts);`);

    // API Endpoint: GET /agents/:agentId
    const response = await fetch(`${baseUrl}/${resolvedAgentId}`);
    if (!response.ok) {
      logger.error(`Failed to get agent`);
      process.exit(1);
    }

    const responseData = await safeJsonParse<ApiResponse<Agent>>(response);
    if (!responseData) {
      throw new Error('Failed to parse agent data from server response');
    }

    const agent = responseData.data;
    if (!agent) {
      throw new Error('No agent data received from server');
    }

    // Save to file if output option is specified - exit early
    if (opts.output !== undefined) {
      // Extract config without metadata fields
      const { id, createdAt, updatedAt, enabled, ...agentConfig } = agent;

      // Create filename with appropriate .json extension
      const filename =
        opts.output === true
          ? `${agent.name || 'agent'}.json`
          : `${String(opts.output)}${String(opts.output).endsWith('.json') ? '' : '.json'}`;

      // Save file and exit
      const jsonPath = path.resolve(process.cwd(), filename);
      writeFileSync(jsonPath, JSON.stringify(agentConfig, null, 2));
      console.log(`Saved agent configuration to ${jsonPath}`);
      return;
    }

    // Display agent details if not using output option
    displayAgent(agent, 'Agent Details');

    // Display JSON if requested
    if (opts.json) {
      const { id, createdAt, updatedAt, enabled, ...agentConfig } = agent;
      console.log(JSON.stringify(agentConfig, null, 2));
    }

    return;
  } catch (error) {
    await checkServer(opts);
    handleError(error);
  }
}

/**
 * Remove command implementation - deletes an agent
 */
std::future<void> removeAgent(OptionValues opts);`);

    // API Endpoint: DELETE /agents/:agentId
    const response = await fetch(`${baseUrl}/${resolvedAgentId}`, {
      method: 'DELETE',
    });

    if (!response.ok) {
      await handleErrorResponse(response, `Failed to remove agent: ${response.statusText}`);
    }

    // Server returns 204 No Content for successful deletion, no need to parse response
    console.log(`Successfully removed agent ${opts.name}`);
    return;
  } catch (error) {
    await checkServer(opts);
    handleError(error);
  }
}

/**
 * Clear memories command implementation - clears all memories for an agent
 */
std::future<void> clearAgentMemories(OptionValues opts);`);

    // API Endpoint: DELETE /agents/:agentId/memories
    const response = await fetch(`${baseUrl}/${resolvedAgentId}/memories`, {
      method: 'DELETE',
    });

    if (!response.ok) {
      await handleErrorResponse(response, `Failed to clear agent memories: ${response.statusText}`);
    }

    const data = await safeJsonParse<ApiResponse<{ deletedCount: number }>>(response);
    const result = data?.data || null;

    console.log(
      `Successfully cleared ${result?.deletedCount || 0} memories for agent ${opts.name}`
    );
    return;
  } catch (error) {
    await checkServer(opts);
    handleError(error);
  }
}

/**
 * Set command implementation - updates agent configuration
 */
std::future<void> setAgentConfig(OptionValues opts);`);

    let config: Record<string, unknown>;
    if (opts.config) {
      try {
        config = JSON.parse(opts.config);
      } catch (error) {
        throw new Error(
          `Failed to parse config JSON string: ${error instanceof Error ? error.message : String(error)}`
        );
      }
    } else if (opts.file) {
      try {
        config = JSON.parse(readFileSync(opts.file, 'utf8'));
      } catch (error) {
        throw new Error(
          `Failed to read or parse config file: ${error instanceof Error ? error.message : String(error)}`
        );
      }
    } else {
      throw new Error('Please provide either a config JSON string (-c) or a config file path (-f)');
    }

    // API Endpoint: PATCH /agents/:agentId
    const response = await fetch(`${getAgentsBaseUrl(opts)}/${resolvedAgentId}`, {
      method: 'PATCH',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(config),
    });

    if (!response.ok) {
      await handleErrorResponse(
        response,
        `Failed to update agent configuration: ${response.statusText}`
      );
    }

    const data = await safeJsonParse<ApiResponse<{ id: string }>>(response);
    const result = data?.data || null;

    console.log(`Successfully updated configuration for agent ${result?.id || resolvedAgentId}`);
  } catch (error) {
    await checkServer(opts);
    handleError(error);
  }
}

} // namespace elizaos
