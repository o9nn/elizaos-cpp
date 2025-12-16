#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;
import type { IntervalMetrics } from "@/app/[interval]/[[...date]]/queries";

struct IssuesListModalContentProps {
    IntervalMetrics["topIssues"] issues;
};


default : IssuesListModalContentProps) {
  return (
    <ScrollArea className="max-h-[80svh]">
      <div className="divide-y px-0">
        {issues.map((issue) => (
          <ActivityItem
            key={issue.id}
            id={issue.id}
            title={issue.title}
            className="px-4"
            author={issue.author}
            number={issue.number}
            href={`https://github.com/${issue.repository}/issues/${issue.number}`}
            icon={
              issue.state === "closed" || issue.closedAt ? (
                <CheckCircle className="h-4 w-4 text-green-500" />
              ) : (
                <CircleDot className="h-4 w-4 text-amber-500" />
              )
            }
            metadata={`${issue.commentCount} comments`}
          />
        ))}
        {issues.length === 0 && (
          <p className="p-4 text-center text-sm text-muted-foreground">
            No issues to display.
          </p>
        )}
      </div>
    </ScrollArea>
  );
}

} // namespace elizaos
