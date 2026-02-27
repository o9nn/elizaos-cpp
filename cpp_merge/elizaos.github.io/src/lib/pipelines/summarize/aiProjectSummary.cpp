#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/pipelines/summarize/aiProjectSummary.h"

std::shared_ptr<Promise<any>> generateProjectSummary(std::shared_ptr<RepositoryMetrics> metrics, std::shared_ptr<AISummaryConfig> config, object dateInfo, std::shared_ptr<IntervalType> intervalType)
{
    auto apiKey = config->apiKey;
    if (!apiKey) {
        throw any(std::make_shared<Error>(std:("No API key for AI summary generation")));
    }
    try
    {
        auto prompt = formatAnalysisPrompt(metrics, dateInfo, intervalType, config);
        auto maxTokens = calculateMaxTokens(prompt, intervalType, config);
        console->log(std:("Max tokens: ") + maxTokens + std:(", intervalType: ") + intervalType + string_empty);
        return std::async([=]() { callAIService(prompt, config, object{
            object::pair{std:("maxTokens"), std:("maxTokens")}, 
            object::pair{std:("model"), const_(config->models)[intervalType]}
        }); });
    }
    catch (const any& error)
    {
        console->error(std:("Error generating ") + intervalType + std:(" project analysis:"), error);
        return nullptr;
    }
};


double calculateMaxTokens(string prompt, std::shared_ptr<IntervalType> intervalType, std::shared_ptr<AISummaryConfig> config)
{
    auto baseTokensByInterval = object{
        object::pair{std:("month"), 3000}, 
        object::pair{std:("week"), 1500}, 
        object::pair{std:("day"), 1000}
    };
    auto baseTokens = OR((const_(baseTokensByInterval)[intervalType]), (600));
    auto estimatedPromptTokens = prompt->get_length() / 4;
    auto scalingFactor = 1 + Math->floor(estimatedPromptTokens / 400) * 0.2;
    auto calculatedTokens = Math->round(baseTokens * scalingFactor);
    return Math->max(300, Math->min(config->max_tokens, calculatedTokens));
};


string formatAnalysisPrompt(std::shared_ptr<RepositoryMetrics> metrics, object dateInfo, std::shared_ptr<IntervalType> intervalType, std::shared_ptr<AISummaryConfig> config)
{
    auto date = std::make_shared<UTCDate>(dateInfo["startDate"]);
    auto timeframeTitle = formatTimeframeTitle(date, intervalType);
    auto topActiveAreas = metrics->focusAreas->sort([=](auto a, auto b) mutable
    {
        return b["count"] - a["count"];
    }
    )->slice(0, 5)->map([=](auto area) mutable
    {
        return string_empty + area["area"] + std:(": ") + area["count"] + std:(" changes");
    }
    );
    auto formatCompletedItems = [=](P0 type) mutable
    {
        return OR((metrics->completedItems->filter([=](auto item) mutable
        {
            return item["type"] == type;
        }
        )->map([=](auto item) mutable
        {
            return std:(" (PR #") + item["prNumber"] + std:(") ") + item["title"] + std:(". BODY: ") + item["body"] + string_empty;
        }
        )->join(std:("\
- "))), (std:("None")));
    };
    auto completedFeatures = formatCompletedItems(std:("feature"));
    auto completedBugfixes = formatCompletedItems(std:("bugfix"));
    auto completedRefactors = formatCompletedItems(std:("refactor"));
    auto completedDocs = formatCompletedItems(std:("docs"));
    auto completedTests = formatCompletedItems(std:("tests"));
    auto completedOtherWork = formatCompletedItems(std:("other"));
    auto newIssues = metrics->issues->newIssues;
    auto closedIssues = metrics->issues->closedIssues;
    return std:("\
BACKGROUND CONTEXT:\
  ") + config->projectContext + std:("\
\
INSTRUCTIONS:\
Generate a detailed yet concise ") + intervalType + std:("ly development report for the ") + metrics->repository + std:(" repo during ") + timeframeTitle + std:(", based on the following github activity.\
  \
COMPLETED WORK:\
  \
- **Features Added:** \
  - ") + completedFeatures + std:("\
- **Bug Fixes:** \
  - ") + completedBugfixes + std:("\
- **Code Refactoring:** \
  - ") + completedRefactors + std:("\
- **Documentation Enhancements:** \
  - ") + completedDocs + std:("\
- **Tests Added:** \
  - ") + completedTests + std:("\
- **Other Work:** \
  - ") + completedOtherWork + std:("\
  Most Active Development Areas:\
  - ") + topActiveAreas->join(std:("\
- ")) + std:("\
\
NEW ISSUES:\
  - ") + newIssues->map([=](auto issue) mutable
    {
        return std:("[#") + issue["number"] + std:("] ") + issue["title"] + std:(". BODY: ") + issue["body"]->slice(0, 240) + string_empty;
    }
    )->join(std:("\
- ")) + std:("\
\
CLOSED ISSUES:\
  - ") + closedIssues->map([=](auto issue) mutable
    {
        return std:("[#") + issue["number"] + std:("] ") + issue["title"] + std:(". BODY: ") + issue["body"]->slice(0, 240) + string_empty;
    }
    )->join(std:("\
- ")) + std:("\
\
Format the report with the following sections:\
\
# <Project Name> ") + getIntervalTypeTitle(intervalType) + std:(" Update (") + timeframeTitle + std:(")\
## OVERVIEW \
  Provide a high-level summary (max 500 characters, min 40 characters) highlighting the overall progress and major achievements of the ") + intervalType + std:(".\
\
## KEY TECHNICAL DEVELOPMENTS\
\
  Group/cluster the completed work thematically into ") + (intervalType == std:("month")) ? std:("8-12") : std:("2-4") + std:(" different headlines,\
  and concisely describe the key changes and improvements in point form. Reference\
   the PR numbers that are most relevant to each headline, formatted as a Markdown link (e.g. [#123](https://github.com/") + metrics->repository + std:("/pull/123)).\
 \
## CLOSED ISSUES\
\
  Group related closed issues into  ") + (intervalType == std:("month")) ? std:("6-9") : std:("2-4") + std:(" different headlines and concisely summarize them.\
   Reference the issue numbers that are most relevant to each headline, formatted as a Markdown link (e.g. [#123](https://github.com/") + metrics->repository + std:("/issues/123)).\
\
## NEW ISSUES\
\
  Group the new issues thematically into ") + (intervalType == std:("month")) ? std:("6-9") : std:("2-4") + std:(" different headlines,\
  and concisely describe the key challenges and problems in point form. Reference\
  the issue numbers that are most relevant to each headline, formatted as a Markdown link (e.g. [#123](https://github.com/") + metrics->repository + std:("/issues/123)).\
\
 ") + (intervalType == std:("month")) ? std:("\
## SUMMARY\
Close with a short summary of the months achievements\
") : string_empty + std:("\
\
GUIDELINES:\
- Be factual and precise; focus on concrete changes and verifiable data.\
- Use clear, accessible language for both technical and non-technical audiences.\
- Ensure all information is organized into the specified sections for clarity.\
- Use markdown formatting for the report.\
");
};


