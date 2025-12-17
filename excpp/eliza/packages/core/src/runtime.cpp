#include "runtime.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void if() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    (this.adapter).runMigrations(p.schema, p.name);
    this.logger.info(`Successfully migrated plugin: ${p.name}`);

}

void normalizeAction(const std::string& actionString) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return actionString.toLowerCase().replace('_', '');

}

} // namespace elizaos
