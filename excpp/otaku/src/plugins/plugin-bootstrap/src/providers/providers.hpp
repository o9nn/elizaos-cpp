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

import type { IAgentRuntime, Memory, Provider, State } from '@elizaos/core';
;

/**
 * Provider for retrieving list of all data providers available for the agent to use.
 * @type { Provider }
 */
/**
 * Object representing the providersProvider, which contains information about data providers available for the agent.
 *
 * @type {Provider}
 * @property {string} name - The name of the provider ("PROVIDERS").
 * @property {string} description - Description of the provider.
 * @property {Function} get - Async  runtime - The runtime of the agent.
 * @param {Memory} _message - The memory message.
 * @returns {Object} An object containing the formatted text and data for potential programmatic use.
 */
const providersProvider: Provider = {
  name: 'PROVIDERS',
  description: 'List of all data providers the agent can use to get additional information',
  get: async (runtime: IAgentRuntime, _message: Memory, _state: State) => {
    const allProviders = runtime.providers;

    // Filter providers with dynamic: true
    const dynamicProviders = allProviders.filter((provider) => provider.dynamic === true);

    // Create formatted text for each provider
    const dynamicDescriptions = dynamicProviders.map((provider) => {
      return `- **${provider.name}**: ${provider.description || 'No description available'}`;
    });

    const allDescriptions = allProviders.map((provider) => {
      return `- **${provider.name}**: ${provider.description || 'No description available'}`;
    });

    // Create the header text
    const headerText =
      '# Providers\n\nThese providers are available for the agent to select and use:';

    const dynamicSection =
      dynamicDescriptions.length > 0
        ? addHeader(headerText, dynamicDescriptions.join('\n'))
        : addHeader(headerText, 'No dynamic providers are currently available.');

    const providersWithDescriptions = addHeader(
      '# Available Providers (External data providers or sources that supply market info)',
      allDescriptions.join('\n')
    );

    const data = {
      dynamicProviders: dynamicProviders.map((provider) => ({
        name: provider.name,
        description: provider.description || '',
      })),
      allProviders: allProviders.map((provider) => ({
        name: provider.name,
        description: provider.description || '',
        dynamic: provider.dynamic === true,
      })),
    };

    const values = {
      providersWithDescriptions,
    };

    return {
      text: dynamicSection,
      data,
      values,
    };
  },
};

} // namespace elizaos
