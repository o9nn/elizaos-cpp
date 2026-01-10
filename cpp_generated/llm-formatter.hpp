#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_LLM-FORMATTER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_LLM-FORMATTER_H
#include "core.h"
#include "@/app/[interval]/[[...date]]/queries.h"

class FormatOptions;

class FormatOptions : public object, public std::enable_shared_from_this<FormatOptions> {
public:
    using std::enable_shared_from_this<FormatOptions>::shared_from_this;
    boolean includeStats;

    boolean includeSummary;

    boolean includePrData;

    boolean includeIssueData;

    boolean includeDetailedContributorSummaries;
};

string sanitizeBodyText(any text, double maxLength = 500);

string formatDataForLLM(std::shared_ptr<IntervalMetrics> metrics, any summaryContent, std::shared_ptr<FormatOptions> options);

#endif
