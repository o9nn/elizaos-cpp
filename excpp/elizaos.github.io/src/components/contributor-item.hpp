#include "stat-badge.hpp"
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
;
;
;

struct ContributorItemProps {
    std::string username;
    std::string href;
    React.ReactNode stats;
    std::optional<std::string> className;
    std::optional<React.ReactNode> children;
    double score;
};


: ContributorItemProps) {
  return (
    <div
      className={cn(
        "flex flex-col gap-3 rounded-lg border p-4 transition-colors hover:bg-accent/50",
        className,
      )}
    >
      <Link href={href} className="">
        <div className="flex gap-3">
          <Avatar className="h-10 w-10">
            <AvatarImage
              src={`https://github.com/${username}.png`}
              alt={username}
            />
            <AvatarFallback>{username[0].toUpperCase()}</AvatarFallback>
          </Avatar>
          <div>
            <div className="flex items-start gap-2 pb-1">
              <h4 className="font-medium">{username}</h4>
              <StatBadge
                labelStart={score.toFixed(0)}
                value="XP"
                variant="secondary"
              />
            </div>
            <div className="text-xs text-muted-foreground">{stats}</div>
          </div>
        </div>
      </Link>

      {children}
    </div>
  );
}

} // namespace elizaos
