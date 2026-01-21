#include "metric-item.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void MetricItem(auto value, auto label, auto valueClassName, auto iconClassName, auto iconBgClassName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div className="flex items-center gap-3">;
    <div;
    className={cn(;
    "flex h-10 w-10 items-center justify-center rounded-full",
    iconBgClassName,
    )}
    >;
    <Icon className={cn("h-5 w-5", iconClassName)} />;
    </div>;
    <div>;
    <div className={cn("text-xl font-bold", valueClassName)}>{value}</div>;
    <div className="text-xs text-muted-foreground">{label}</div>;
    </div>;
    </div>;
    );

}

} // namespace elizaos
