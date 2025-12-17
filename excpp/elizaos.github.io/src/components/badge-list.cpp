#include "badge-list.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void BadgeList(auto {
  items, auto className, bg-emerald-500/20 dark:text-emerald-500" badgeClassName = "border-0 bg-emerald-500/15 text-emerald-600 hover, BadgeListProps }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div className={cn("flex flex-wrap gap-2", className)}>;
    {items.map((item) => (;
    <Badge key={item.id} className={cn(badgeClassName, item.className)}>;
    {item.label}
    </Badge>;
    ))}
    </div>;
    );

}

} // namespace elizaos
