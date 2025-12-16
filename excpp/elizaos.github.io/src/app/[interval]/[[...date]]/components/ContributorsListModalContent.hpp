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

struct Contributor {
    std::string username;
    double totalScore;
    std::optional<string | null> summary;
};


struct ContributorsListModalContentProps {
    std::vector<Contributor> contributors;
};


default : ContributorsListModalContentProps) {
  return (
    <ScrollArea className="max-h-[80svh]">
      <div className="divide-y px-0">
        {contributors.map((contributor) => (
          <div key={contributor.username}>
            <ContributorItem
              className="border-none px-4"
              username={contributor.username}
              score={contributor.totalScore}
              href={`/profile/${contributor.username}`}
              stats={
                <p className="whitespace-pre-wrap break-words">
                  {contributor.summary?.replace(
                    `${contributor.username}: `,
                    "",
                  )}
                </p>
              }
            ></ContributorItem>
          </div>
        ))}
        {contributors.length === 0 && (
          <p className="p-4 text-center text-sm text-muted-foreground">
            No contributors to display.
          </p>
        )}
      </div>
    </ScrollArea>
  );
}

} // namespace elizaos
