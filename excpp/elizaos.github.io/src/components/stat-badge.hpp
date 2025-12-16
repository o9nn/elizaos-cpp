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

interface StatBadgeProps extends BadgeProps {
  labelStart?: string;
  labelEnd?: string;
  value: string | number;
}

: StatBadgeProps) {
  return (
    <Badge variant={variant} className={cn("gap-1", className)} {...props}>
      {labelStart ? (
        <span className="text-primary/80">{labelStart}</span>
      ) : null}
      <span className="text-muted-foreground">{value}</span>
      {labelEnd ? <span className="text-primary/80">{labelEnd}</span> : null}
    </Badge>
  );
}

} // namespace elizaos
