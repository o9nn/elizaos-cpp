#include "types.hpp"
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
;

/**
 * Creates a pipeline step to generate time intervals for a repository with a specific interval type
 */
const generateTimeIntervals = <TInput extends Record<string, unknown>>(
  intervalType: IntervalType,
) =>
  createStep<
    TInput,
    Array<TInput & { interval: TimeInterval }>,
    RepoPipelineContext
  >(
    "generateTimeIntervals",
    (input: TInput, { dateRange: dateRangeInput, logger, config }) => {
      const defaultStartDate = toDateString(subDays(new UTCDate(), 7));
      const dateRange = {
        startDate:
          dateRangeInput?.startDate ||
          config.contributionStartDate ||
          defaultStartDate,
        endDate: dateRangeInput?.endDate,
      };

      logger?.debug("Generating time intervals", {
        dateRange,
        intervalType,
      });

      const intervals = generateTimeIntervalsForDateRange(
        intervalType,
        dateRange,
      );

      logger?.debug(`Generated ${intervals.length} intervals`, {
        intervals: intervals.map((interval) => ({
          intervalStart: toDateString(interval.intervalStart),
          intervalEnd: toDateString(interval.intervalEnd),
        })),
      });

      return intervals.map((interval) => ({ ...input, interval }));
    },
  );

} // namespace elizaos
