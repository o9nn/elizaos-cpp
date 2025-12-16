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

class Logger {
  private prefix = '[StagehandServer]';

  info(message: string, ...args: any[]) {
    console.log(`${this.prefix} INFO:`, message, ...args);
  }

  debug(message: string, ...args: any[]) {
    if (process.env.DEBUG) {
      console.log(`${this.prefix} DEBUG:`, message, ...args);
    }
  }

  error(message: string, error?: any) {
    console.error(`${this.prefix} ERROR:`, message, error);
  }

  warn(message: string, ...args: any[]) {
    console.warn(`${this.prefix} WARN:`, message, ...args);
  }
}

} // namespace elizaos
