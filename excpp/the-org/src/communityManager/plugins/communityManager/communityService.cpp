#include "communityService.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string extractString(unknown value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto result = '';

    function traverse(node: unknown): void {
        if (typeof node == 'string') {
            result += node;
            } else if (Array.isArray(node)) {
                for (const auto& item : node)
                    traverse(item);
                }
                } else if (typeof node == 'object' && node != nullptr) {
                    for (const auto& val : Object.values(node)
                        traverse(val);
                    }
                }
            }

            traverse(value);
            return result.trim();

}

} // namespace elizaos
