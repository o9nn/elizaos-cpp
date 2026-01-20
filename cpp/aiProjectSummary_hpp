#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_PIPELINES_SUMMARIZE_AIPROJECTSUMMARY_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_PIPELINES_SUMMARIZE_AIPROJECTSUMMARY_H
#include "core.h"
#include "./config.h"
#include "./callAIService.h"
#include "../codeAreaHelpers.h"
#include "../export/queries.h"
#include "@date-fns/utc.h"
#include "@/lib/date-utils.h"

class CompletedItem;
class FocusArea;
class PullRequestMetrics;
class IssueMetrics;
class CodeChangeMetrics;
class ContributorInfo;
class ProjectMetricsForSummary;

class CompletedItem : public object, public std::enable_shared_from_this<CompletedItem> {
public:
    using std::enable_shared_from_this<CompletedItem>::shared_from_this;
    string title;

    double prNumber;

    WorkItemType type;
};

class FocusArea : public object, public std::enable_shared_from_this<FocusArea> {
public:
    using std::enable_shared_from_this<FocusArea>::shared_from_this;
    string area;

    double count;
};

class PullRequestMetrics : public object, public std::enable_shared_from_this<PullRequestMetrics> {
public:
    using std::enable_shared_from_this<PullRequestMetrics>::shared_from_this;
    double total;

    double merged;

    double open;

    array<any> items;

    double mergedThisPeriod;
};

class IssueMetrics : public object, public std::enable_shared_from_this<IssueMetrics> {
public:
    using std::enable_shared_from_this<IssueMetrics>::shared_from_this;
    double total;

    double opened;

    double closed;

    array<any> items;

    array<any> closedThisPeriod;
};

class CodeChangeMetrics : public object, public std::enable_shared_from_this<CodeChangeMetrics> {
public:
    using std::enable_shared_from_this<CodeChangeMetrics>::shared_from_this;
    double additions;

    double deletions;

    double files;

    double commitCount;
};

class ContributorInfo : public object, public std::enable_shared_from_this<ContributorInfo> {
public:
    using std::enable_shared_from_this<ContributorInfo>::shared_from_this;
    string username;
};

class ProjectMetricsForSummary : public object, public std::enable_shared_from_this<ProjectMetricsForSummary> {
public:
    using std::enable_shared_from_this<ProjectMetricsForSummary>::shared_from_this;
    std::shared_ptr<PullRequestMetrics> pullRequests;

    std::shared_ptr<IssueMetrics> issues;

    double uniqueContributors;

    array<std::shared_ptr<ContributorInfo>> topContributors;

    std::shared_ptr<CodeChangeMetrics> codeChanges;

    array<std::shared_ptr<FocusArea>> focusAreas;

    array<std::shared_ptr<CompletedItem>> completedItems;
};

std::shared_ptr<Promise<any>> generateProjectSummary(std::shared_ptr<RepositoryMetrics> metrics, std::shared_ptr<AISummaryConfig> config, object dateInfo, std::shared_ptr<IntervalType> intervalType);

double calculateMaxTokens(string prompt, std::shared_ptr<IntervalType> intervalType, std::shared_ptr<AISummaryConfig> config);

string formatAnalysisPrompt(std::shared_ptr<RepositoryMetrics> metrics, object dateInfo, std::shared_ptr<IntervalType> intervalType, std::shared_ptr<AISummaryConfig> config);

#endif
