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

// Initialize SQLite database
let sqlite: Database.Database;
try {
  sqlite = new Database(path.join(process.cwd(), "data/db.sqlite"), {
    // Enable WAL mode for better performance
    fileMustExist: false,
  });

  // Enable WAL mode for better performance
  sqlite.pragma("journal_mode = WAL");
} catch (error) {
  console.error("Failed to initialize database:", error);
  process.exit(1);
}

// Create drizzle database instance
const db = drizzle(sqlite, { schema });

// Ensure database is closed when the process exits
process.on("exit", () => {
  sqlite.close();
});

} // namespace elizaos
