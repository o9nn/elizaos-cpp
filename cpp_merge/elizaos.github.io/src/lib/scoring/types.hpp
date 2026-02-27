#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_SCORING_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_SCORING_TYPES_H
#include "core.h"
#include "drizzle-orm.h"
#include "zod.h"
#include "@/lib/data/schema.h"

typedef any AggregationPeriod;

typedef z::infer<UserScoreMetricsSchema> UserScoreMetrics;

typedef any UserScoreWithMetrics;

enum struct TagType;

extern any UserScoreMetricsSchema;
enum struct TagType {
    AREA = std:("AREA"), ROLE = std:("ROLE"), TECH = std:("TECH")
};
#endif
