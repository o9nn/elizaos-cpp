#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/llm-formatter.h"

string sanitizeBodyText(any text, double maxLength)
{
    if (!text) return string_empty;
    auto processedText = text->replace((new RegExp(std::string("<!--[\s\S]*?-->"))), string_empty);
    processedText = processedText->trim();
    processedText = processedText->replace((new RegExp(std::string("\r\n|\n|\r"))), std::string("\n"));
    return processedText->slice(0, maxLength);
};


string formatDataForLLM(std::shared_ptr<IntervalMetrics> metrics, any summaryContent, std::shared_ptr<FormatOptions> options)
{
    shared parts = array<string>();
    parts->push(std::string("## Metadata"));
    parts->push(std::string("```json"));
    parts->push(std::string("{"));
    parts->push(std::string("  "interval_type": "") + metrics->interval->intervalType + std::string("","));
    parts->push(std::string("  "start_date": "") + metrics->interval->intervalStart + std::string("","));
    parts->push(std::string("  "end_date": "") + metrics->interval->intervalEnd + std::string("""));
    parts->push(std::string("}"));
    parts->push(std::string("```"));
    parts->push(string_empty);
    if (options->includeStats) {
        parts->push(std::string("## Statistics"));
        parts->push(std::string("```json"));
        parts->push(std::string("{"));
        parts->push(std::string("  "pull_requests": {"));
        parts->push(std::string("    "new": ") + metrics->pullRequests->new + std::string(","));
        parts->push(std::string("    "merged": ") + metrics->pullRequests->merged + std::string(","));
        parts->push(std::string("    "total_unique": ") + metrics->pullRequests->total + string_empty);
        parts->push(std::string("  },"));
        parts->push(std::string("  "issues": {"));
        parts->push(std::string("    "new": ") + metrics->issues->new + std::string(","));
        parts->push(std::string("    "closed": ") + metrics->issues->closed + std::string(","));
        parts->push(std::string("    "total_unique": ") + metrics->issues->total + string_empty);
        parts->push(std::string("  },"));
        parts->push(std::string("  "contributors": {"));
        parts->push(std::string("    "active": ") + metrics->activeContributors + string_empty);
        parts->push(std::string("  },"));
        parts->push(std::string("  "code_changes": {"));
        parts->push(std::string("    "commits": ") + metrics->codeChanges->commitCount + std::string(","));
        parts->push(std::string("    "files_changed": ") + metrics->codeChanges->files + std::string(","));
        parts->push(std::string("    "lines_added": ") + metrics->codeChanges->additions->toLocaleString() + std::string(","));
        parts->push(std::string("    "lines_deleted": ") + metrics->codeChanges->deletions->toLocaleString() + string_empty);
        parts->push(std::string("  }"));
        parts->push(std::string("}"));
        parts->push(std::string("```"));
        parts->push(string_empty);
    }
    if (AND((options->includeSummary), (summaryContent))) {
        parts->push(std::string("## Summary"));
        parts->push(summaryContent);
        parts->push(string_empty);
    }
    if (AND((metrics->topContributors), (metrics->topContributors->length > 0))) {
        parts->push(std::string("## Top Contributors"));
        parts->push(std::string("```json"));
        parts->push(std::string("/* Top contributors for this period */"));
        parts->push(std::string("["));
        metrics->topContributors->forEach([=](auto contributor, auto index) mutable
        {
            auto comma = (index < metrics->topContributors->length - 1) ? std::string(",") : string_empty;
            parts->push(std::string("  {"));
            parts->push(std::string("    "username": "") + contributor["username"] + std::string("","));
            parts->push(std::string("    "score": ") + contributor["totalScore"] + string_empty);
            parts->push(std::string("  }") + comma + string_empty);
        }
        );
        parts->push(std::string("]"));
        parts->push(std::string("```"));
        parts->push(string_empty);
    }
    if (AND((AND((options->includeDetailedContributorSummaries), (metrics->detailedContributorSummaries))), (Object->keys(metrics->detailedContributorSummaries)->get_length() > 0))) {
        parts->push(std::string("## Detailed Contributor Summaries"));
        parts->push(string_empty);
        console->log(std::string("[formatDataForLLM] Including detailed contributor summaries. Found ") + Object->keys(metrics->detailedContributorSummaries)->get_length() + std::string(" summaries."));
        for (auto& [username, summary] : Object->entries(metrics->detailedContributorSummaries))
        {
            if (AND((summary), (summary->trim() != string_empty))) {
                parts->push(std::string("### ") + username + string_empty);
                parts->push(summary);
                parts->push(string_empty);
            }
        }
    } else {
        if (options->includeDetailedContributorSummaries) {
            console->log(std::string("[formatDataForLLM] Option to include detailed contributor summaries was true, but no summaries were found or metrics.detailedContributorSummaries was empty."));
        } else {
            console->log(std::string("[formatDataForLLM] Option to include detailed contributor summaries was false."));
        }
    }
    if (AND((AND((options->includePrData), (metrics->topPullRequests))), (metrics->topPullRequests->length > 0))) {
        parts->push(std::string("## Pull Requests"));
        parts->push(std::string("```json"));
        parts->push(std::string("["));
        metrics->topPullRequests->forEach([=](auto pr, auto index) mutable
        {
            auto comma = (index < metrics->topPullRequests->length - 1) ? std::string(",") : string_empty;
            parts->push(std::string("  {"));
            parts->push(std::string("    "number": ") + pr["number"] + std::string(","));
            parts->push(std::string("    "title": "") + pr["title"]["replace"]((new RegExp(std::string("""))), std::string("\"")) + std::string("","));
            parts->push(std::string("    "author": "") + pr["author"] + std::string("","));
            parts->push(std::string("    "status": "") + (pr["mergedAt"]) ? std::string("Merged") : std::string("New") + std::string("","));
            parts->push(std::string("    "link": "https://github.com/") + pr["repository"] + std::string("/pull/") + pr["number"] + std::string("","));
            parts->push(std::string("    "body": "") + sanitizeBodyText(OR((pr["body"]), (std::string("Body not available."))), 750)->replace((new RegExp(std::string("""))), std::string("\"")) + std::string("""));
            parts->push(std::string("  }") + comma + string_empty);
        }
        );
        parts->push(std::string("]"));
        parts->push(std::string("```"));
        parts->push(string_empty);
    }
    if (AND((AND((options->includeIssueData), (metrics->topIssues))), (metrics->topIssues->length > 0))) {
        parts->push(std::string("## Issues"));
        parts->push(std::string("```json"));
        parts->push(std::string("["));
        metrics->topIssues->forEach([=](auto issue, auto index) mutable
        {
            auto comma = (index < metrics->topIssues->length - 1) ? std::string(",") : string_empty;
            parts->push(std::string("  {"));
            parts->push(std::string("    "number": ") + issue["number"] + std::string(","));
            parts->push(std::string("    "title": "") + issue["title"]["replace"]((new RegExp(std::string("""))), std::string("\"")) + std::string("","));
            parts->push(std::string("    "author": "") + issue["author"] + std::string("","));
            parts->push(std::string("    "state": "") + issue["state"] + std::string("","));
            parts->push(std::string("    "comments": ") + issue["commentCount"] + std::string(","));
            parts->push(std::string("    "link": "https://github.com/") + issue["repository"] + std::string("/issues/") + issue["number"] + std::string("","));
            parts->push(std::string("    "body": "") + sanitizeBodyText(OR((issue["body"]), (std::string("Body not available."))))->replace((new RegExp(std::string("""))), std::string("\"")) + std::string("""));
            parts->push(std::string("  }") + comma + string_empty);
        }
        );
        parts->push(std::string("]"));
        parts->push(std::string("```"));
        parts->push(string_empty);
    }
    return parts->join(std::string("\
"));
};


