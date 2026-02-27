#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/pipelines/contributors/index.h"

std::shared_ptr<PipelineStep<any, array<object>, std::shared_ptr<ContributorPipelineContext>>> processContributorTags = pipe(fetchAllContributors, mapStep(calculateTags), createStep(std::string("logResults"), [=](auto results, auto context) mutable
{
    auto totalContributors = results->filter(isNotNullOrUndefined)->get_length();
    context->logger->info(std::string("Processed tags for ") + totalContributors + std::string(" contributors"));
    return results;
}
));
std::shared_ptr<PipelineStep<std::shared_ptr<Record<string, any>>, array<object>, std::shared_ptr<RepoPipelineContext>>> processContributorScores = pipe(generateTimeIntervals(std::string("day")), mapStep(pipe(getActiveContributorsInInterval, processContributorsForInterval)), createStep(std::string("logScoringResults"), [=](auto intervals, auto context) mutable
{
    auto totalIntervals = intervals->get_length();
    auto allUniqueContributors = intervals->filter(isNotNullOrUndefined)->flatMap([=](auto interval) mutable
    {
        return interval["results"]->map([=](auto r) mutable
        {
            return r["username"];
        }
        );
    }
    );
    auto uniqueContributors = array<any>{ std::make_shared<Set>(allUniqueContributors) };
    context->logger->info(std::string("Processed scores for ") + uniqueContributors->get_length() + std::string(" unique contributors over ") + totalIntervals + std::string(" days"));
    return intervals;
}
));
std::shared_ptr<PipelineStep<any, std::tuple<array<object>, array<object>>, std::shared_ptr<RepoPipelineContext>>> contributorsPipeline = sequence(processContributorTags, processContributorScores);

void Main(void)
{
}

MAIN
