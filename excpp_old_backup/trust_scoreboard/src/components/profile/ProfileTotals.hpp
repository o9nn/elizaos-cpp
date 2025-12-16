#include "ProfileTotals.module.css.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using View = std::variant<'profile', 'holdings'>;

struct ProfileTotalsProps {
    std::optional<(view: View) => void> onViewChange;
};

struct MetricsData {
    number | null trustScore;
    number | null successRate;
    number | null transparency;
    number | null shillingScore;
    number | null recommendations;
    number | null rank;
    number | null totalPartners;
    number | null userHoldings;
};


} // namespace elizaos
