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

/**
 * Type definition for logger methods
 */
using LogMethod = (...args: any[]) => void;

// logger wrapper/specification
const logger: Record<
  | "trace"
  | "debug"
  | "success"
  | "progress"
  | "log"
  | "info"
  | "warn"
  | "error"
  | "fatal"
  | "clear",
  LogMethod
> = {
  trace: (...args: any[]) => {
    const [message = "", ...rest] = args;
    return coreLogger.trace.apply(coreLogger, [String(message), ...rest]);
  },
  debug: (...args: any[]) => {
    const [message = "", ...rest] = args;
    return coreLogger.debug.apply(coreLogger, [String(message), ...rest]);
  },
  success: (...args: any[]) => {
    const [message = "", ...rest] = args;
    return coreLogger.debug.apply(coreLogger, [String(message), ...rest]);
  },
  progress: (...args: any[]) => {
    const [message = "", ...rest] = args;
    return coreLogger.debug.apply(coreLogger, [String(message), ...rest]);
  },
  log: (...args: any[]) => {
    const [message = "", ...rest] = args;
    return coreLogger.info.apply(coreLogger, [String(message), ...rest]);
  },
  info: (...args: any[]) => {
    const [message = "", ...rest] = args;
    return coreLogger.info.apply(coreLogger, [String(message), ...rest]);
  },
  warn: (...args: any[]) => {
    const [message = "", ...rest] = args;
    return coreLogger.warn.apply(coreLogger, [String(message), ...rest]);
  },
  error: (...args: any[]) => {
    const [message = "", ...rest] = args;
    return coreLogger.error.apply(coreLogger, [String(message), ...rest]);
  },
  fatal: (...args: any[]) => {
    const [message = "", ...rest] = args;
    return coreLogger.fatal.apply(coreLogger, [String(message), ...rest]);
  },
  clear: (...args: any[]) => {
    const [message = "", ...rest] = args;
    return coreLogger.clear.apply(coreLogger, [String(message), ...rest]);
  },
};

{ logger };

// for backward compatibility
// we should deprecate this
const elizaLogger = logger;

default logger;

} // namespace elizaos
