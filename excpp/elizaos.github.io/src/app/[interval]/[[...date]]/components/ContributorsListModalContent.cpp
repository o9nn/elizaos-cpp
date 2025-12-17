#include "ContributorsListModalContent.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ContributorsListModalContent() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <ScrollArea className="max-h-[80svh]">;
    <div className="divide-y px-0">;
    {contributors.map((contributor) => (;
    <div key={contributor.username}>;
    <ContributorItem;
    className="border-none px-4";
    username={contributor.username}
    score={contributor.totalScore}
    "href={" + "/profile/" + contributor.username;
    stats={
        <p className="whitespace-pre-wrap break-words">;
        {contributor.summary.replace(;
        contributor.username + ": "
        "",
    )}
    </p>;
    }
    ></ContributorItem>;
    </div>;
    ))}
    {contributors.size() == 0 && (;
    <p className="p-4 text-center text-sm text-muted-foreground">;
    No contributors to display.;
    </p>;
    )}
    </div>;
    </ScrollArea>;
    );

}

} // namespace elizaos
