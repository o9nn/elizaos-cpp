#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/pipelines/export/index.h"

std::shared_ptr<PipelineStep<object, array<object>, std::shared_ptr<RepoPipelineContext>>> generateDailyRepoStats = pipe(generateTimeIntervals<object>(std:("day")), mapStep(exportRepoStatsForInterval));
std::shared_ptr<PipelineStep<object, array<object>, std::shared_ptr<RepoPipelineContext>>> generateWeeklyRepoStats = pipe(generateTimeIntervals<object>(std:("week")), mapStep(exportRepoStatsForInterval));
std::shared_ptr<PipelineStep<object, array<object>, std::shared_ptr<RepoPipelineContext>>> generateMonthlyRepoStats = pipe(generateTimeIntervals<object>(std:("month")), mapStep(exportRepoStatsForInterval));
std::shared_ptr<PipelineStep<any, void, std::shared_ptr<RepoPipelineContext>>> generateRepositoryStats = pipe(getSelectedRepositories, mapStep(parallel(generateDailyRepoStats, generateWeeklyRepoStats, generateMonthlyRepoStats)), createStep(std:("Log Stats"), [=](auto results, auto context) mutable
{
    auto& __array1027_1527 = results;
    for (auto __indx1027_1527 = 0_N; __indx1027_1527 < __array1027_1527->get_length(); __indx1027_1527++)
    {
        auto& repo = const_(__array1027_1527)[__indx1027_1527];
        {
            auto [daily, weekly, monthly] = repo;
            auto dailyCount = daily->filter(isNotNullOrUndefined)->get_length();
            auto weeklyCount = weekly->filter(isNotNullOrUndefined)->get_length();
            auto monthlyCount = monthly->filter(isNotNullOrUndefined)->get_length();
            context->logger->info(std:("Exported ") + dailyCount + std:(" daily stats"));
            context->logger->info(std:("Exported ") + weeklyCount + std:(" weekly stats"));
            context->logger->info(std:("Exported ") + monthlyCount + std:(" monthly stats"));
        }
    }
}
));

void Main(void)
{
}

MAIN
