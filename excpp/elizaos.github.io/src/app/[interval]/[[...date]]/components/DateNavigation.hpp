#include "IntervalSelector.hpp"
#include "NavigationButton.hpp"
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

;
;
;

struct DateNavigationProps {
    string | null prevDate;
    string | null nextDate;
    std::string currentDate;
    IntervalType intervalType;
};


: DateNavigationProps) {
  return (
    <div className="mb-6">
      <div className="flex items-center justify-between">
        <NavigationButton
          href={prevDate ? `/${intervalType}/${prevDate}` : "#"}
          direction="prev"
          isVisible={!!prevDate}
        />

        <div className="flex flex-col items-center">
          <div className="text-sm font-medium text-gray-500">
            {getIntervalTypeTitle(intervalType)} Summary
          </div>
          <time dateTime={currentDate} className="text-md font-bold">
            {formatTimeframeTitle(currentDate, intervalType)}
          </time>
        </div>

        <NavigationButton
          href={nextDate ? `/${intervalType}/${nextDate}` : "#"}
          direction="next"
          isVisible={!!nextDate}
        />
      </div>
    </div>
  );
}

} // namespace elizaos
