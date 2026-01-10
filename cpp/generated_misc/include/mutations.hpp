#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_PIPELINES_SUMMARIZE_MUTATIONS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_PIPELINES_SUMMARIZE_MUTATIONS_H
#include "core.h"
#include "@/lib/date-utils.h"
#include "@/lib/data/db.h"
#include "@/lib/data/schema.h"
#include "@date-fns/utc.h"

std::shared_ptr<Promise<void>> storeDailySummary(string username, string date, string summary, std::shared_ptr<IntervalType> intervalType);

std::shared_ptr<Promise<void>> storeRepoSummary(string repoId, string date, string summary, std::shared_ptr<IntervalType> intervalType);

#endif
