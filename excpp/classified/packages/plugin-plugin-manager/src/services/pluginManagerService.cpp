#include "pluginManagerService.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void resetRegistryCache() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    registryCache = nullptr;

}

std::future<std::unordered_map<std::string, RegistryEntry>> getLocalRegistryIndex() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Check cache first
        if (registryCache && Date.now() - registryCache.timestamp < CACHE_DURATION) {
            return registryCache.data;
        }

        const auto response = fetch(REGISTRY_URL);
        if (!response.ok) {
            throw std::runtime_error(`Registry fetch failed: ${response.statusText}`);
        }

        const auto data = (response.json())<string, RegistryEntry>;

        // Cache the result
        registryCache = {
            data,
            timestamp: Date.now(),
            };

            return data;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void if() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    pluginState.plugin.init({}, this.runtime);

}

} // namespace elizaos
