#include "context.hpp"

std::shared_ptr<SummarizerPipelineContext> createSummarizerContext(std::shared_ptr<CreateContributorSummaryContextOptions> options)
{
    return utils::assign(object{
    }, options);
};


