#include "getSelectedRepositories.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> registerRepository(const std::string& owner, const std::string& name) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    db;
    .insert(repositories);
    .values({
        owner,
        name,
        "repoId: " + owner + "/" + name
        lastUpdated: new UTCDate().toISOString(),
        });
        .onConflictDoUpdate({
            target: repositories.repoId,
            set: {
                owner,
                name,
                lastUpdated: new UTCDate().toISOString(),
                },
                });

                return { repoId: `${owner}/${name}` }

}

} // namespace elizaos
