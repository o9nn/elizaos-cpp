#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "IntervalSelector.hpp"
#include "NavigationButton.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct DateNavigationProps {
    std::optional<std::string> prevDate;
    std::optional<std::string> nextDate;
    std::string currentDate;
    IntervalType intervalType;
};


} // namespace elizaos
