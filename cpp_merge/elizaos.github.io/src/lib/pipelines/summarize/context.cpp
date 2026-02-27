#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/pipelines/summarize/context.h"

std::shared_ptr<SummarizerPipelineContext> createSummarizerContext(std::shared_ptr<CreateContributorSummaryContextOptions> options)
{
    return utils::assign(object{
    }, options);
};


