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

;

// Add client-specific context to logs
const clientLogger = {
  info: (msg: string, ...args: any[]) => {
    elizaLogger.info({ source: 'client' }, msg, ...args);
  },
  error: (msg: string, ...args: any[]) => {
    elizaLogger.error({ source: 'client' }, msg, ...args);
  },
  warn: (msg: string, ...args: any[]) => {
    elizaLogger.warn({ source: 'client' }, msg, ...args);
  },
  debug: (msg: string, ...args: any[]) => {
    elizaLogger.debug({ source: 'client' }, msg, ...args);
  },
};

default clientLogger;

} // namespace elizaos
