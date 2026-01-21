#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void initializeDatabase(const std::string& dataDir) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (process.env.POSTGRES_URL) {
        const auto db = new PostgresDatabaseAdapter({;
            connectionString: process.env.POSTGRES_URL,
            });
            return db;
            } else {
                const auto filePath =;
                process.env.SQLITE_FILE || path.resolve(dataDir, "db.sqlite");
                // ":memory:";
                const auto db = new SqliteDatabaseAdapter(new Database(filePath));
                return db;
            }

}

} // namespace elizaos
