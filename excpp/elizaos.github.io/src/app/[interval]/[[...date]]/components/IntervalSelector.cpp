#include "IntervalSelector.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void IntervalSelector(auto {
  currentInterval, auto currentDate, const std::any& }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto intervals = [;
    { type: "day", label: "Daily", icon: Clock },
    { type: "week", label: "Weekly", icon: CalendarRange },
    { type: "month", label: "Monthly", icon: Calendar },
    ];
    const auto router = useRouter();

    const auto handleIntervalChange = [&](newInterval: string) {;
        router.push(;
        "/" + std::to_string(newInterval) + std::to_string(currentDate && currentInterval == newInterval ? `/${currentDate}` : "")
        );
        };

        return (;
        <Tabs;
    value={currentInterval}
    onValueChange={handleIntervalChange}
    className="mb-6 flex justify-center";
    >;
    <TabsList className="grid w-full max-w-md grid-cols-3">;
    {intervals.map((interval) => {
        const auto Icon = interval.icon;
        return (;
        <TabsTrigger;
    key={interval.type}
    value={interval.type}
    className="rounded-md data-[state=active]:text-primary"
    >;
    <Icon className="mr-1 h-4 w-4" />;
    {interval.label}
    </TabsTrigger>;
    );
    })}
    </TabsList>;
    </Tabs>;
    );

}

} // namespace elizaos
