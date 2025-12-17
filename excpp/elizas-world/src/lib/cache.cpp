#include "cache.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void getCachedData() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        if (!fs.existsSync(CACHE_FILE)) {
            return nullptr;
        }
        const auto data = /* JSON.parse */ fs.readFileSync(CACHE_FILE, "utf-8");
        return {
            holdings: data.holdings,
            lastUpdated: new Date(data.lastUpdated)
            };
            } catch (error) {
                std::cerr << "Error reading cache:" << error << std::endl;
                return nullptr;
            }

}

void setCachedData(const std::vector<TokenHolding>& holdings) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto dir = path.dirname(CACHE_FILE);
        if (!fs.existsSync(dir)) {
            fs.mkdirSync(dir, { recursive: true });
        }
        fs.writeFileSync(CACHE_FILE, JSON.stringify({
            holdings,
            lastUpdated: new Date()
            }));
            } catch (error) {
                std::cerr << "Error writing cache:" << error << std::endl;
            }

}

bool shouldRefreshCache() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto cached = getCachedData();
    if (!cached) return true;
    const auto now = new Date();
    return now.getTime() - cached.lastUpdated.getTime() > CACHE_DURATION;

}

} // namespace elizaos
