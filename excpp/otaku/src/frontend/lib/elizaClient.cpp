#include "elizaClient.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void updateApiKey(const std::optional<std::string>& newKey) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (newKey) {
        localStorage.setItem('eliza-api-key', newKey);
        } else {
            localStorage.removeItem('eliza-api-key');
        }

}

} // namespace elizaos
