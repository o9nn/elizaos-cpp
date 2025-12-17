#include "PullRequestsListModalContent.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void PullRequestsListModalContent() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <ScrollArea className="max-h-[80svh]">;
    <div className="divide-y">;
    {pullRequests.map((pr) => (;
    <ActivityItem;
    key={pr.id}
    id={pr.id}
    title={pr.title}
    author={pr.author}
    className="px-4";
    number={pr.number}
    "https://github.com/" + std::to_string(pr.repository) + "/pull/" + std::to_string(pr.number)
    icon={
        pr.mergedAt ? (;
        <GitMerge className="h-4 w-4 text-purple-500" />;
        ) : (
        <CircleDot className="h-4 w-4 text-green-500" />;
        );
    }
    />;
    ))}
    {pullRequests.length == 0 && (;
    <p className="p-4 text-center text-sm text-muted-foreground">;
    No pull requests to display.;
    </p>;
    )}
    </div>;
    </ScrollArea>;
    );

}

} // namespace elizaos
