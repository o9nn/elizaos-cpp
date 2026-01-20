#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_PIPELINES_SUMMARIZE_CONTEXT_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_PIPELINES_SUMMARIZE_CONTEXT_H
#include "core.h"
#include "@/lib/pipelines/pipelineConfig.h"
#include "@/lib/logger.h"
#include "@/lib/pipelines/types.h"
#include "./config.h"
#include "@/lib/date-utils.h"

class SummarizerPipelineContext;
class CreateContributorSummaryContextOptions;

class SummarizerPipelineContext : public RepoPipelineContext, public std::enable_shared_from_this<SummarizerPipelineContext> {
public:
    using std::enable_shared_from_this<SummarizerPipelineContext>::shared_from_this;
    string outputDir;

    std::shared_ptr<AISummaryConfig> aiSummaryConfig;

    std::shared_ptr<Logger> logger;

    boolean overwrite;

    object dateRange;

    Record<std::shared_ptr<IntervalType>, boolean> enabledIntervals;
};

class CreateContributorSummaryContextOptions : public object, public std::enable_shared_from_this<CreateContributorSummaryContextOptions> {
public:
    using std::enable_shared_from_this<CreateContributorSummaryContextOptions>::shared_from_this;
    string repoId;

    std::shared_ptr<PipelineConfig> config;

    std::shared_ptr<Logger> logger;

    string outputDir;

    std::shared_ptr<AISummaryConfig> aiSummaryConfig;

    boolean overwrite;

    object dateRange;

    Record<std::shared_ptr<IntervalType>, boolean> enabledIntervals;
};

std::shared_ptr<SummarizerPipelineContext> createSummarizerContext(std::shared_ptr<CreateContributorSummaryContextOptions> options);

#endif
