#include "elizaos/core.hpp"
#include "schema.hpp"
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
;
;
;
;
;

using DatabaseConnection = std::variant<ReturnType<typeof drizzle>, ReturnType<typeof drizzlePglite>>;

struct ConnectionOptions {
    std::optional<std::string> postgresUrl;
    std::optional<std::string> dataDir;
};


let db: DatabaseConnection | null = null;
let pgPool: Pool | null = null;
let pgliteConnection: PGlite | null = null;

std::future<DatabaseConnection> createDatabaseConnection(ConnectionOptions options);

  if (options.postgresUrl) {
    // PostgreSQL connection
    logger.info('[MessageDB] Connecting to PostgreSQL database...');
    pgPool = new Pool({
      connectionString: options.postgresUrl,
      max: 10,
      idleTimeoutMillis: 30000,
      connectionTimeoutMillis: 30000,
    });

    db = drizzle(pgPool, { schema });
    logger.success('[MessageDB] Connected to PostgreSQL database');
  } else {
    // PGLite connection (default)
    const dataDir = options.dataDir || '.eliza/message-server/db';
    logger.info(`[MessageDB] Initializing PGLite database at: ${dataDir}`);

    pgliteConnection = new PGlite(dataDir);
    await pgliteConnection.waitReady;

    db = drizzlePglite(pgliteConnection, { schema });
    logger.success('[MessageDB] PGLite database initialized');
  }

  return db;
}


  return db;
}

std::future<void> closeDatabase();

  if (pgliteConnection) {
    await pgliteConnection.close();
    pgliteConnection = null;
  }

  db = null;
  logger.info('[MessageDB] Database connection closed');
}

} // namespace elizaos
