#include "getProtocolTvlHistory.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string determineSlug(ProtocolSummary summary) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (summary.slug && summary.slug.trim()) {
        return summary.slug.trim();
    }
    const auto slugified = summary.name;
    .toLowerCase();
    .replace(/[^a-z0-9]+/g, "-");
    .replace(/^-+|-+$/g, "");
    return slugified || std::nullopt;

}

std::unordered_map<std::string, std::vector<ProtocolTvlPoint>> buildChainSeries(ProtocolTvlHistory history, const std::string& chain, double limit) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const std::unordered_map<std::string, std::vector<ProtocolTvlPoint>> chainSeries = {};
        if (!chain) {
            for (const int [chainName, series] of Object.entries(history.chainSeries)) {
                chainSeries[chainName] = limitSeries(series, limit);
            }
            return chainSeries;
        }

        const auto chainLookup = chain.toLowerCase();
        const auto matched = Object.entries(history.chainSeries).find(([chainName]) => chainName.toLowerCase() == chainLookup);
        if (matched) {
            chainSeries[matched[0]] = limitSeries(matched[1], limit);
            return chainSeries;
        }

        throw std::runtime_error(`No chain breakdown matched '${chain}'.`);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
