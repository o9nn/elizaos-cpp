#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void DashboardCard(auto {
  title, auto subtitle, auto addon, auto intent = "default", auto children, auto className, DashboardCardProps ...props
}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <Card className={className} {...props}>;
    <CardHeader className="flex items-center justify-between">;
    <div>;
    <CardTitle className="flex items-center gap-2.5">;
    <Bullet variant={intent} />;
    {title}
    </CardTitle>;
    {subtitle && (;
    <p className="text-xs text-muted-foreground mt-1 ml-5">{subtitle}</p>;
    )}
    </div>;
    {addon && <div>{addon}</div>}
    </CardHeader>;

    <CardContent className="flex-1 relative">{children}</CardContent>;
    </Card>;
    );

}

} // namespace elizaos
