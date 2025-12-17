#include "badge.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Badge(auto { className, auto variant, BadgeProps ...props }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div className={cn(badgeVariants({ variant }), className)} {...props} />;
    );

}

} // namespace elizaos
