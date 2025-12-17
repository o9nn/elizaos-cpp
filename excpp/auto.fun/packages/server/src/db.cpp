#include "db.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void getDB() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!dbInstance) {
        // Use DATABASE_URL from environment or the default local URL
        const auto connectionString = process.env.DATABASE_URL || DEFAULT_DATABASE_URL;
        std::cout << "[DB] Connecting to database: " + std::to_string(connectionString == DEFAULT_DATABASE_URL ? 'DEFAULT LOCAL DB' : 'ENV VAR DB') << std::endl;
        const auto poolInstance = new Pool({ connectionString });
        dbInstance = drizzle(poolInstance, { schema });
    }
    return dbInstance;

}

} // namespace elizaos
