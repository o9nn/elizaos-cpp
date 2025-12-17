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



/**
 * Class representing a database client manager for PGlite.
 * @implements { IDatabaseClientManager }
 */
class PGliteClientManager implements IDatabaseClientManager<PGlite> {
  private client: PGlite;
  private shuttingDown = false;

  /**
   * Constructor for creating a new instance of PGlite with the provided options.
   * Initializes the PGlite client with additional extensions.
   * @param {PGliteOptions} options - The options to configure the PGlite client.
   */
  constructor(options: PGliteOptions) {
    this.client = new PGlite({
      ...options,
      extensions: {
        vector,
        fuzzystrmatch,
      },
    });
    this.setupShutdownHandlers();
  }

    // Kept for backward compatibility

    // Implementation of setupShutdownHandlers method

} // namespace elizaos
