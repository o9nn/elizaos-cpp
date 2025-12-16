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

import type { IAgentRuntime, Memory, Provider } from '@elizaos/core';

/**
 * Time provider 
 */
const timeProvider: Provider = {
  name: 'TIME',
  get: async (_runtime: IAgentRuntime, _message: Memory) => {
    const currentDate = new Date();

    // Get UTC time since bots will be communicating with users around the global
    const options = {
      timeZone: 'UTC',
      dateStyle: 'full' as const,
      timeStyle: 'long' as const,
    };
    const humanReadable = new Intl.DateTimeFormat('en-US', options).format(currentDate);
    return {
      data: {
        time: currentDate,
      },
      values: {
        time: humanReadable,
      },
      text: `The current date and time is ${humanReadable}. Please use this as your reference for any time-based operations or responses.`,
    };
  },
};

} // namespace elizaos
