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
        time: `🚨 CRITICAL: The current date and time is ${humanReadable}. You MUST use this exact time as your absolute reference for ALL time-based operations, calculations, and responses. DO NOT use any other time reference. This is the ONLY source of truth for current time.`,
      },
      text: `🚨 CRITICAL: The current date and time is ${humanReadable}. You MUST use this exact time as your absolute reference for ALL time-based operations, calculations, and responses. DO NOT use any other time reference. This is the ONLY source of truth for current time.`,
    };
  },
};

} // namespace elizaos
