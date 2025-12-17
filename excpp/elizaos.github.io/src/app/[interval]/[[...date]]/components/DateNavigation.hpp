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
    std::string prevDate;
    std::string nextDate;
    std::string currentDate;
    IntervalType intervalType;
};

void DateNavigation(auto nextDate, auto currentDate, auto intervalType);

} // namespace elizaos
