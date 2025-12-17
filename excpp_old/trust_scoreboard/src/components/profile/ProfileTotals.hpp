#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "ProfileTotals.module.css.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using View = std::variant<'profile', 'holdings'>;

struct ProfileTotalsProps {
};

struct MetricsData {
    std::optional<double> trustScore;
    std::optional<double> successRate;
    std::optional<double> transparency;
    std::optional<double> shillingScore;
    std::optional<double> recommendations;
    std::optional<double> rank;
    std::optional<double> totalPartners;
    std::optional<double> userHoldings;
};


} // namespace elizaos
