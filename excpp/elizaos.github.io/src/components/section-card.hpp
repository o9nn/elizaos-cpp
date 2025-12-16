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

struct SectionCardProps {
    std::string title;
    std::optional<LucideIcon> icon;
    React.ReactNode children;
    std::optional<std::string> className;
    std::optional<std::string> contentClassName;
    std::optional<std::string> headerClassName;
    std::optional<bool> noPadding;
};


: SectionCardProps) {
  return (
    <Card className={className}>
      <CardHeader className={headerClassName}>
        <CardTitle className="text-sm font-medium">
          {Icon && (
            <span className="flex items-center gap-2">
              <Icon className="h-4 w-4" /> {title}
            </span>
          )}
          {!Icon && title}
        </CardTitle>
      </CardHeader>
      <CardContent
        className={cn(noPadding ? "px-0 py-0" : "", contentClassName)}
      >
        {children}
      </CardContent>
    </Card>
  );
}

} // namespace elizaos
