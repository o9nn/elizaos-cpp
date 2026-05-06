#include "section-card.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void SectionCard(Icon icon, auto children, auto className, auto contentClassName, auto headerClassName, auto noPadding) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <Card className={className}>;
    <CardHeader className={headerClassName}>;
    <CardTitle className="text-sm font-medium">;
    {Icon && (;
    <span className="flex items-center gap-2">;
    <Icon className="h-4 w-4" /> {title}
    </span>;
    )}
    {!Icon && title}
    </CardTitle>;
    </CardHeader>;
    <CardContent;
    className={cn(noPadding ? "px-0 py-0" : "", contentClassName)}
    >;
    {children}
    </CardContent>;
    </Card>;
    );

}

} // namespace elizaos
