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

struct BadgeItem {
    string | number id;
    std::string label;
    std::optional<std::string> className;
};


struct BadgeListProps {
    std::vector<BadgeItem> items;
    std::optional<std::string> className;
    std::optional<std::string> badgeClassName;
};


: BadgeListProps) {
  return (
    <div className={cn("flex flex-wrap gap-2", className)}>
      {items.map((item) => (
        <Badge key={item.id} className={cn(badgeClassName, item.className)}>
          {item.label}
        </Badge>
      ))}
    </div>
  );
}

} // namespace elizaos
