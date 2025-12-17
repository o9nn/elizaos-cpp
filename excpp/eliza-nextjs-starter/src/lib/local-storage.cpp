#include "local-storage.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<std::string> getOrGenerateSeed() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof window == "undefined" || !window.localStorage) {
        std::cout << "Local Storage not available. Cannot get or generate seed." << std::endl;
        return nullptr;
    }

    try {
        auto seed = localStorage.getItem(SEED_KEY);
        if (!seed) {
            std::cout << "Generating new persistence seed." << std::endl;
            // Use crypto.randomUUID() for a strong random ID
            seed = crypto.randomUUID();
            localStorage.setItem(SEED_KEY, seed);
        }
        return seed;
        } catch (error) {
            std::cerr << "Error accessing Local Storage for seed:" << error << std::endl;
            return nullptr;
        }

}

} // namespace elizaos
