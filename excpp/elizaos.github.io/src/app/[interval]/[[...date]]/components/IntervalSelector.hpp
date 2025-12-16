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

"use client";
;
;
;
;
;
;

/**
 * Component for switching between different interval views
 */
: {
  currentInterval: IntervalType;
  currentDate: string;
}) {
  const intervals = [
    { type: "day" as const, label: "Daily", icon: Clock },
    { type: "week" as const, label: "Weekly", icon: CalendarRange },
    { type: "month" as const, label: "Monthly", icon: Calendar },
  ];
  const router = useRouter();

  const handleIntervalChange = (newInterval: string) => {
    router.push(
      `/${newInterval}${currentDate && currentInterval === newInterval ? `/${currentDate}` : ""}`,
    );
  };

  return (
    <Tabs
      value={currentInterval}
      onValueChange={handleIntervalChange}
      className="mb-6 flex justify-center"
    >
      <TabsList className="grid w-full max-w-md grid-cols-3">
        {intervals.map((interval) => {
          const Icon = interval.icon;
          return (
            <TabsTrigger
              key={interval.type}
              value={interval.type}
              className="rounded-md data-[state=active]:text-primary"
            >
              <Icon className="mr-1 h-4 w-4" />
              {interval.label}
            </TabsTrigger>
          );
        })}
      </TabsList>
    </Tabs>
  );
}

} // namespace elizaos
