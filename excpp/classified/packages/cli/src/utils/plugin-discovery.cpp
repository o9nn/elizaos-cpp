#include "plugin-discovery.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::optional<CachedRegistry>> fetchPluginRegistry() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto resp = fetch(;
            'https://raw.githubusercontent.com/elizaos-plugins/registry/refs/heads/main/generated-registry.json'
            );
            if (!resp.ok) {
                std::cerr << "Failed to fetch plugin registry: " + std::to_string(resp.statusText) << std::endl;
                throw std::runtime_error(`Failed to fetch registry: ${resp.statusText}`);
            }
            const auto raw = resp.json();
            return raw;
            } catch {
                return nullptr;
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
