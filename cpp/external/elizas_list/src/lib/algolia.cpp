#include "algolia.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> indexProject(const std::any& project) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    index.saveObject({
        objectID: project.id,
        ...project,
        });

}

std::future<void> removeProject(const std::string& projectId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    index.deleteObject(projectId);

}

} // namespace elizaos
