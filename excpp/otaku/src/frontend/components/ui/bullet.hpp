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
import React, { HTMLAttributes } from "react";

const bulletVariants = cva("rounded-[1.5px]", {
  variants: {
    variant: {
      default: "bg-primary",
      success: "bg-success",
      warning: "bg-warning",
      destructive: "bg-destructive",
    },
    size: {
      sm: "size-2",
      default: "size-2.5",
      lg: "size-3",
    },
  },
  defaultVariants: {
    variant: "default",
    size: "default",
  },
});

interface BulletProps
  extends VariantProps<typeof bulletVariants>,
    HTMLAttributes<HTMLDivElement> {}

const Bullet = ({ variant, size, className, ...props }: BulletProps) => {
  return (
    <div
      className={cn(bulletVariants({ variant, size }), className)}
      {...props}
    ></div>
  );
};

} // namespace elizaos
