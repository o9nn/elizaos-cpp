#include "DateNavigation.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void DateNavigation(auto nextDate, auto currentDate, auto intervalType) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div className="mb-6">;
    <div className="flex items-center justify-between">;
    <NavigationButton;
    "/" + std::to_string(intervalType) + "/" + std::to_string(prevDate)
    direction="prev";
    isVisible={!!prevDate}
    />;

    <div className="flex flex-col items-center">;
    <div className="text-sm font-medium text-gray-500">;
    {getIntervalTypeTitle(intervalType)} Summary;
    </div>;
    <time dateTime={currentDate} className="text-md font-bold">;
    {formatTimeframeTitle(currentDate, intervalType)}
    </time>;
    </div>;

    <NavigationButton;
    "/" + std::to_string(intervalType) + "/" + std::to_string(nextDate)
    direction="next";
    isVisible={!!nextDate}
    />;
    </div>;
    </div>;
    );

}

} // namespace elizaos
