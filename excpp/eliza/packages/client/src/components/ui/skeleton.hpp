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

interface SkeletonProps extends React.HTMLAttributes<HTMLDivElement> {
  'data-testid'?: string;
}

: SkeletonProps) {
  return (
    <div
      className={cn('animate-pulse rounded-md bg-primary/10', className)}
      data-testid={props['data-testid'] || 'skeleton'}
      {...props}
    />
  );
}

{ Skeleton };

} // namespace elizaos
