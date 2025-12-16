#include ".utils.hpp"
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
import type { VariantProps } from 'class-variance-authority';
;
import type { HTMLAttributes } from 'react';
;

const badgeVariants = cva(
  'inline-flex items-center select-none rounded-md border px-2.5 py-0.5 text-xs font-semibold transition-colors focus:outline-none focus:ring-2 focus:ring-ring focus:ring-offset-2',
  {
    variants: {
      variant: {
        default: 'border-transparent bg-primary text-primary-foreground shadow hover:bg-primary/80',
        secondary:
          'border-transparent bg-secondary text-secondary-foreground hover:bg-secondary/80',
        destructive: 'border-transparent bg-red-200 text-red-700 shadow hover:bg-red-200/80',
        success: 'border-transparent bg-green-200 text-green-700 shadow hover:bg-green-200/80',
        outline: 'text-foreground',
      },
    },
    defaultVariants: {
      variant: 'default',
    },
  }
);

interface BadgeProps
  extends HTMLAttributes<HTMLDivElement>,
    VariantProps<typeof badgeVariants> {}

: BadgeProps) {
  return <div className={cn(badgeVariants({ variant }), className)} {...props} />;
}

{ Badge, badgeVariants };

} // namespace elizaos
