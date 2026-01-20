#include "badge-list.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void BadgeList(auto className, auto badgeClassName) {
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
