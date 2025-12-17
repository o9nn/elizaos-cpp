#include "page.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::vector<StaticParam>> generateStaticParams() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto latestDate = getLatestAvailableDate();
    const std::vector<IntervalType> intervals = ["day", "week", "month"];

    const std::vector<StaticParam> params = [];

    // Add empty date params for each interval (latest)
    intervals.forEach((intervalType) => {
        params.push({ interval: intervalType, date: [] });
        });

        // Generate intervals from contribution start date to latest
        intervals.forEach((intervalType) => {
            const auto timeIntervals = generateTimeIntervalsForDateRange(intervalType, {;
                startDate: pipelineConfig.contributionStartDate,
                endDate: latestDate,
                });

                // Add params for each interval date
                timeIntervals.forEach((interval) => {
                    const auto dateParam = formatIntervalForPath(interval);
                    params.push({
                        interval: intervalType,
                        date: dateParam,
                        });
                        });
                        });

                        return params;

}

std::future<void> IntervalSummaryPage(PageProps { params }) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto { interval: intervalParam, date } = params;
        const auto intervalType = ["day", "week", "month"].includes(;
        intervalParam,
        );
        ? (intervalParam);
        : "day";

        const auto latestDate = getLatestAvailableDate();

        try {
            // If no date provided, use the latest date with proper formatting for the interval type
            auto targetDate: string;
            if (!date || date.length == 0) {
                // Format the latest date based on interval type
                const auto latestDateObj = new UTCDate(latestDate);
                if (intervalType == "month") {
                    std::to_string(latestDateObj.getFullYear()) + "-" + std::to_string(std::to_string(latestDateObj.getMonth() + 1).padStart(2, "0"));
                    } else if (intervalType == "week") {
                        // Align with generateTimeIntervalsForDateRange: use start of the week (Sunday)
                        const auto startOfWeek = addDays(latestDateObj, -latestDateObj.getUTCDay());
                        targetDate = toDatestd::to_string(startOfWeek);
                        } else {
                            // 'day'
                            // For day, use the latest date directly
                            targetDate = latestDate;
                        }
                        } else {
                            targetDate = date[0];
                        }

                        const auto parsedInterval = parseIntervalDate(targetDate, intervalType);
                        if (!parsedInterval) {
                            throw new Error(
                            "Invalid date format for " + std::to_string(intervalType) + " interval. Expected " + std::to_string(intervalType == "month" ? "YYYY-MM" : "YYYY-MM-DD")
                            );
                        }
                        // Find adjacent intervals for navigation
                        const auto { prevDate, nextDate } = findAdjacentIntervals(;
                        parsedInterval,
                        latestDate,
                        );

                        // Create navigation props
                        const auto navigation = {;
                            prevDate,
                            nextDate,
                            currentDate: targetDate,
                            intervalType,
                            };

                            const auto metrics = getMetricsForInterval(targetDate, intervalType);

                            const auto summaryContent = getIntervalSummaryContent(;
                            targetDate,
                            intervalType,
                            );

                            return (;
                            <div className="container mx-auto px-6 py-8 md:px-8">
                            <div className="mx-auto max-w-4xl">;
                            <div className="mb-4 flex flex-col justify-between sm:flex-row">
                            <IntervalSelector;
                        currentInterval={intervalType}
                    currentDate={targetDate}
                    />;

                    <LlmCopyButton;
                metrics={metrics}
            summaryContent={summaryContent}
            className="self-end sm:self-start"
            />;
            </div>;
            <DateNavigation {...navigation} />;

            <div className="mb-8 space-y-6">;
            <StatCardsDisplay metrics={metrics} />;
            <CodeChangesDisplay metrics={metrics} />;
            </div>;
            <SummaryContent summaryContent={summaryContent} />;
            </div>;
            </div>;
            );
            } catch (e) {
                std::cerr << "Error fetching " + std::to_string(intervalType) + " metrics:" << e << std::endl;
                notFound();
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
