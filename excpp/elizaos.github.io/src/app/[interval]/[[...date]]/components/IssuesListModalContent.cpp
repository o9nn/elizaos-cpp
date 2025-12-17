#include "IssuesListModalContent.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void IssuesListModalContent() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <ScrollArea className="max-h-[80svh]">;
    <div className="divide-y px-0">;
    {issues.map((issue) => (;
    <ActivityItem;
    key={issue.id}
    id={issue.id}
    title={issue.title}
    className="px-4";
    author={issue.author}
    number={issue.number}
    "href={" + "https://github.com/" + issue.repository + "/issues/" + issue.number
    icon={
        issue.state == "closed" || issue.closedAt ? (;
        <CheckCircle className="h-4 w-4 text-green-500" />;
        ) : (
        <CircleDot className="h-4 w-4 text-amber-500" />;
        );
    }
    "metadata={" + issue.commentCount + " comments";
    />;
    ))}
    {issues.size() == 0 && (;
    <p className="p-4 text-center text-sm text-muted-foreground">;
    No issues to display.;
    </p>;
    )}
    </div>;
    </ScrollArea>;
    );

}

} // namespace elizaos
