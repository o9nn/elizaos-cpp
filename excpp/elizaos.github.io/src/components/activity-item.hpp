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

struct ActivityItemProps {
    std::string id;
    std::string title;
    std::string author;
    double number;
    std::string href;
    React.ReactNode icon;
    std::optional<React.ReactNode> metadata;
    std::optional<std::string> className;
};


: ActivityItemProps) {
  return (
    <a
      key={id}
      href={href}
      target="_blank"
      rel="noopener noreferrer"
      className={cn(
        "flex items-start gap-3 p-4 transition-colors hover:bg-muted/50",
        className,
      )}
    >
      <div className="mt-0.5 flex h-6 w-6 items-center justify-center rounded-full">
        {icon}
      </div>
      <div className="flex-1">
        <div className="flex items-center justify-between">
          <h4 className="mr-2 line-clamp-1 font-medium">{title}</h4>
          <div className="flex-shrink-0">
            <ExternalLink className="h-4 w-4 text-muted-foreground" />
          </div>
        </div>
        <div className="mt-1 flex items-center gap-2">
          <Avatar className="h-4 w-4">
            <AvatarImage
              src={`https://github.com/${author}.png`}
              alt={author}
            />
            <AvatarFallback>{author[0].toUpperCase()}</AvatarFallback>
          </Avatar>
          <span className="text-xs text-muted-foreground">
            {author} • #{number} {metadata && <> • {metadata}</>}
          </span>
        </div>
      </div>
    </a>
  );
}

} // namespace elizaos
