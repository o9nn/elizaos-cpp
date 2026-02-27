#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAOS_GITHUB_IO_SRC_LIB_SCORING_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAOS_GITHUB_IO_SRC_LIB_SCORING_TYPES_H
#include "core.hpp"
#include <string>
#include "drizzle-orm.h"
#include "zod.hpp"
// External dependency removed

typedef any AggregationPeriod;

typedef z::infer<UserScoreMetricsSchema> UserScoreMetrics;

typedef any UserScoreWithMetrics;

enum struct TagType;

extern any UserScoreMetricsSchema;
enum struct TagType {
    AREA = std::string("AREA"), ROLE = std::string("ROLE"), TECH = std::string("TECH")
};
#endif
