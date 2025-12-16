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

struct MetricItemProps {
    LucideIcon icon;
    string | number value;
    std::string label;
    std::optional<std::string> valueClassName;
    std::optional<std::string> iconClassName;
    std::optional<std::string> iconBgClassName;
};


: MetricItemProps) {
  return (
    <div className="flex items-center gap-3">
      <div
        className={cn(
          "flex h-10 w-10 items-center justify-center rounded-full",
          iconBgClassName,
        )}
      >
        <Icon className={cn("h-5 w-5", iconClassName)} />
      </div>
      <div>
        <div className={cn("text-xl font-bold", valueClassName)}>{value}</div>
        <div className="text-xs text-muted-foreground">{label}</div>
      </div>
    </div>
  );
}

} // namespace elizaos
