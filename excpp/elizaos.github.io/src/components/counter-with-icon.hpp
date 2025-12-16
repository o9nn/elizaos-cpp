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

struct CounterWithIconProps {
    LucideIcon icon;
    std::string label;
    number | string value;
    std::optional<std::string> iconClassName;
    std::optional<std::string> className;
};


: CounterWithIconProps) {
  return (
    <div className={cn("flex items-center gap-2", className)}>
      <Icon className={cn("h-4 w-4", iconClassName)} />
      <span className="text-sm text-muted-foreground">{label}</span>
      <span className="font-bold">{value}</span>
    </div>
  );
}

} // namespace elizaos
