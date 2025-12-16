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

struct MigrationMeta {
    std::vector<std::string> sql;
    bool bps;
    double folderMillis;
    std::string hash;
};


/**
 * Custom migration `);
  }

  const journalAsString = fs.readFileSync(journalPath).toString();
  const journal = JSON.parse(journalAsString);

  let journalEntries = journal.entries;

  if (maxMigrationNumber !== undefined) {
    console.log(`Filtering migrations up to number: ${maxMigrationNumber}`);
    journalEntries = journal.entries.filter((entry) => {
      return entry.idx <= maxMigrationNumber;
    });
  }

  console.log(`Found ${journalEntries.length} migrations to apply.`);

  if (journalEntries.length === 0) {
    console.log("No migrations to apply.");
    return;
  }

  const migrationQueries: MigrationMeta[] = [];
  for (const journalEntry of journalEntries) {
    const migrationPath = path.join(
      config.migrationsFolder,
      `${journalEntry.tag}.sql`,
    );
    try {
      const query = fs.readFileSync(migrationPath).toString();
      const result = query
        .split("--> statement-breakpoint")
        .map((it) => it.trim())
        .filter((it) => it.length > 0);

      migrationQueries.push({
        sql: result,
        bps: journalEntry.breakpoints,
        folderMillis: journalEntry.when,
        hash: crypto.createHash("sha256").update(query).digest("hex"),
      });
    } catch (e) {
      throw new Error(
        `Error reading migration file ${migrationPath}: ${e.message}`,
      );
    }
  }

  db.dialect.migrate(migrationQueries, db.session, config);
};

const dbPath = process.env.DB_PATH || "./data/db.sqlite";
console.log(`Using database at: ${dbPath}`);

// Ensure the directory for the database exists
const dbDir = path.dirname(dbPath);
if (!fs.existsSync(dbDir)) {
  fs.mkdirSync(dbDir, { recursive: true });
}

const sqlite = new Database(dbPath, { create: true });
const db = drizzle(sqlite);

const maxMigrationArg = process.argv[2];
let maxMigration: number | undefined;

if (maxMigrationArg) {
  const parsed = parseInt(maxMigrationArg, 10);
  if (isNaN(parsed)) {
    console.error(
      `Error: Invalid migration number provided: "${maxMigrationArg}". Must be an integer.`,
    );
    process.exit(1);
  }
  maxMigration = parsed;
}

try {
  customMigrate(db, { migrationsFolder: "./drizzle" }, maxMigration);
  console.log("Migration complete");
} catch (error) {
  console.error("Migration failed:", error);
  process.exit(1);
}

} // namespace elizaos
