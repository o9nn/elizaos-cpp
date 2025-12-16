#include "components/CodeChangesDisplay.hpp"
#include "components/DateNavigation.hpp"
#include "components/IntervalSelector.hpp"
#include "components/StatCardsDisplay.hpp"
#include "components/SummaryContent.hpp"
#include "queries.hpp"
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



struct PageProps {
    Promise<{ params;
    std::string interval;
    std::optional<std::vector<std::string>> date;
};

struct StaticParam {
    std::string interval;
    std::optional<std::vector<std::string>> date;
};


} // namespace elizaos
