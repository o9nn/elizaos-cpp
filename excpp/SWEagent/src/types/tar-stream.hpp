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

declare module 'tar-stream' {
  /**
   * Create a tar pack stream
   * @returns A writable stream for creating tar archives
   */
  function pack(): NodeJS.WritableStream;

  /**
   * Create a tar extract stream
   * @returns A writable stream for extracting tar archives
   */
  function extract(): NodeJS.WritableStream;
}

} // namespace elizaos
