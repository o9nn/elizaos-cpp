#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAOS_GITHUB_IO_SRC_LIB_PIPELINES_CONTRIBUTORS_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAOS_GITHUB_IO_SRC_LIB_PIPELINES_CONTRIBUTORS_INDEX_H
#include "core.hpp"
#include "../types.h"
#include "./calculateTags.h"
#include "./fetchAllContributors.h"
#include "../generateTimeIntervals.h"
#include "./contributorScores.h"
#include "../getActiveContributors.h"
// External dependency removed

extern std::shared_ptr<PipelineStep<any, array<object>, std::shared_ptr<ContributorPipelineContext>>> processContributorTags;
extern std::shared_ptr<PipelineStep<std::shared_ptr<Record<string, any>>, array<object>, std::shared_ptr<RepoPipelineContext>>> processContributorScores;
extern std::shared_ptr<PipelineStep<any, std::tuple<array<object>, array<object>>, std::shared_ptr<RepoPipelineContext>>> contributorsPipeline;
#endif
