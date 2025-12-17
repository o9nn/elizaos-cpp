#include "stat-badge.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void StatBadge(auto labelEnd, auto value, auto variant, auto className) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <Badge variant={variant} className={cn("gap-1", className)} {...props}>;
    {labelStart ? (;
    <span className="text-primary/80">{labelStart}</span>;
    ) : nullptr}
    <span className="text-muted-foreground">{value}</span>;
    {labelEnd ? <span className="text-primary/80">{labelEnd}</span>  = nullptr}
    </Badge>;
    );

}

} // namespace elizaos
