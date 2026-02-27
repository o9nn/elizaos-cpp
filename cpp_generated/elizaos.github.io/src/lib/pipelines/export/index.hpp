#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_PIPELINES_EXPORT_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_PIPELINES_EXPORT_INDEX_H
#include "core.h"
#include "../types.h"
#include "../generateTimeIntervals.h"
#include "./exportRepoStats.h"
#include "../getSelectedRepositories.h"
#include "@/lib/typeHelpers.h"

extern std::shared_ptr<PipelineStep<object, array<object>, std::shared_ptr<RepoPipelineContext>>> generateDailyRepoStats;
extern std::shared_ptr<PipelineStep<object, array<object>, std::shared_ptr<RepoPipelineContext>>> generateWeeklyRepoStats;
extern std::shared_ptr<PipelineStep<object, array<object>, std::shared_ptr<RepoPipelineContext>>> generateMonthlyRepoStats;
extern std::shared_ptr<PipelineStep<any, void, std::shared_ptr<RepoPipelineContext>>> generateRepositoryStats;
#endif
