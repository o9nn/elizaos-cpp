#include "factory.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void setDatabaseType(DatabaseType dbType) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    globalFactory = new SchemaFactory(dbType);

}

SchemaFactory getSchemaFactory() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!globalFactory) {
        // Default to postgres for backward compatibility
        globalFactory = new SchemaFactory("postgres");
    }
    return globalFactory;

}

} // namespace elizaos
