#include "llm-formatter.h"

string sanitizeBodyText(any text, double maxLength)
{
    if (!text) return string_empty;
    auto processedText = text->replace((new RegExp(std:("<!--[\s\S]*?-->"))), string_empty);
    processedText = processedText->trim();
    processedText = processedText->replace((new RegExp(std:("\r\n|\n|\r"))), std:("\n"));
    return processedText->slice(0, maxLength);
};


string formatDataForLLM(std::shared_ptr<IntervalMetrics> metrics, any summaryContent, std::shared_ptr<FormatOptions> options)
{
    shared parts = array<string>();
    parts->push(std:("## Metadata"));
    parts->push(std:(""""json"));
    parts->push(std:("{"));
    parts->push(std:("  "interval_type": "") + metrics->interval->intervalType + std:("","));
    parts->push(std:("  "start_date": "") + metrics->interval->intervalStart + std:("","));
    parts->push(std:("  "end_date": "") + metrics->interval->intervalEnd + std:("""));
    parts->push(std:("}"));
    parts->push(std:("""""));
    parts->push(string_empty);
    if (options->includeStats) {
        parts->push(std:("## Statistics"));
        parts->push(std:(""""json"));
        parts->push(std:("{"));
        parts->push(std:("  "pull_requests": {"));
        parts->push(std:("    "new": ") + metrics->pullRequests->new + std:(","));
        parts->push(std:("    "merged": ") + metrics->pullRequests->merged + std:(","));
        parts->push(std:("    "total_unique": ") + metrics->pullRequests->total + string_empty);
        parts->push(std:("  },"));
        parts->push(std:("  "issues": {"));
        parts->push(std:("    "new": ") + metrics->issues->new + std:(","));
        parts->push(std:("    "closed": ") + metrics->issues->closed + std:(","));
        parts->push(std:("    "total_unique": ") + metrics->issues->total + string_empty);
        parts->push(std:("  },"));
        parts->push(std:("  "contributors": {"));
        parts->push(std:("    "active": ") + metrics->activeContributors + string_empty);
        parts->push(std:("  },"));
        parts->push(std:("  "code_changes": {"));
        parts->push(std:("    "commits": ") + metrics->codeChanges->commitCount + std:(","));
        parts->push(std:("    "files_changed": ") + metrics->codeChanges->files + std:(","));
        parts->push(std:("    "lines_added": ") + metrics->codeChanges->additions->toLocaleString() + std:(","));
        parts->push(std:("    "lines_deleted": ") + metrics->codeChanges->deletions->toLocaleString() + string_empty);
        parts->push(std:("  }"));
        parts->push(std:("}"));
        parts->push(std:("""""));
        parts->push(string_empty);
    }
    if (AND((options->includeSummary), (summaryContent))) {
        parts->push(std:("## Summary"));
        parts->push(summaryContent);
        parts->push(string_empty);
    }
    if (AND((metrics->topContributors), (metrics->topContributors->length > 0))) {
        parts->push(std:("## Top Contributors"));
        parts->push(std:(""""json"));
        parts->push(std:("/* Top contributors for this period */"));
        parts->push(std:("["));
        metrics->topContributors->forEach([=](auto contributor, auto index) mutable
        {
            auto comma = (index < metrics->topContributors->length - 1) ? std:(",") : string_empty;
            parts->push(std:("  {"));
            parts->push(std:("    "username": "") + contributor["username"] + std:("","));
            parts->push(std:("    "score": ") + contributor["totalScore"] + string_empty);
            parts->push(std:("  }") + comma + string_empty);
        }
        );
        parts->push(std:("]"));
        parts->push(std:("""""));
        parts->push(string_empty);
    }
    if (AND((AND((options->includeDetailedContributorSummaries), (metrics->detailedContributorSummaries))), (Object->keys(metrics->detailedContributorSummaries)->get_length() > 0))) {
        parts->push(std:("## Detailed Contributor Summaries"));
        parts->push(string_empty);
        console->log(std:("[formatDataForLLM] Including detailed contributor summaries. Found ") + Object->keys(metrics->detailedContributorSummaries)->get_length() + std:(" summaries."));
        for (auto& [username, summary] : Object->entries(metrics->detailedContributorSummaries))
        {
            if (AND((summary), (summary->trim() != string_empty))) {
                parts->push(std:("### ") + username + string_empty);
                parts->push(summary);
                parts->push(string_empty);
            }
        }
    } else {
        if (options->includeDetailedContributorSummaries) {
            console->log(std:("[formatDataForLLM] Option to include detailed contributor summaries was true, but no summaries were found or metrics.detailedContributorSummaries was empty."));
        } else {
            console->log(std:("[formatDataForLLM] Option to include detailed contributor summaries was false."));
        }
    }
    if (AND((AND((options->includePrData), (metrics->topPullRequests))), (metrics->topPullRequests->length > 0))) {
        parts->push(std:("## Pull Requests"));
        parts->push(std:(""""json"));
        parts->push(std:("["));
        metrics->topPullRequests->forEach([=](auto pr, auto index) mutable
        {
            auto comma = (index < metrics->topPullRequests->length - 1) ? std:(",") : string_empty;
            parts->push(std:("  {"));
            parts->push(std:("    "number": ") + pr["number"] + std:(","));
            parts->push(std:("    "title": "") + pr["title"]["replace"]((new RegExp(std:("""))), std:("\"")) + std:("","));
            parts->push(std:("    "author": "") + pr["author"] + std:("","));
            parts->push(std:("    "status": "") + (pr["mergedAt"]) ? std:("Merged") : std:("New") + std:("","));
            parts->push(std:("    "link": "https://github.com/") + pr["repository"] + std:("/pull/") + pr["number"] + std:("","));
            parts->push(std:("    "body": "") + sanitizeBodyText(OR((pr["body"]), (std:("Body not available."))), 750)->replace((new RegExp(std:("""))), std:("\"")) + std:("""));
            parts->push(std:("  }") + comma + string_empty);
        }
        );
        parts->push(std:("]"));
        parts->push(std:("""""));
        parts->push(string_empty);
    }
    if (AND((AND((options->includeIssueData), (metrics->topIssues))), (metrics->topIssues->length > 0))) {
        parts->push(std:("## Issues"));
        parts->push(std:(""""json"));
        parts->push(std:("["));
        metrics->topIssues->forEach([=](auto issue, auto index) mutable
        {
            auto comma = (index < metrics->topIssues->length - 1) ? std:(",") : string_empty;
            parts->push(std:("  {"));
            parts->push(std:("    "number": ") + issue["number"] + std:(","));
            parts->push(std:("    "title": "") + issue["title"]["replace"]((new RegExp(std:("""))), std:("\"")) + std:("","));
            parts->push(std:("    "author": "") + issue["author"] + std:("","));
            parts->push(std:("    "state": "") + issue["state"] + std:("","));
            parts->push(std:("    "comments": ") + issue["commentCount"] + std:(","));
            parts->push(std:("    "link": "https://github.com/") + issue["repository"] + std:("/issues/") + issue["number"] + std:("","));
            parts->push(std:("    "body": "") + sanitizeBodyText(OR((issue["body"]), (std:("Body not available."))))->replace((new RegExp(std:("""))), std:("\"")) + std:("""));
            parts->push(std:("  }") + comma + string_empty);
        }
        );
        parts->push(std:("]"));
        parts->push(std:("""""));
        parts->push(string_empty);
    }
    return parts->join(std:("\
"));
};


