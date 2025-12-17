#include "counter-with-icon.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void CounterWithIcon(Icon {
  icon, auto label, auto value, auto iconClassName, auto className, CounterWithIconProps }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div className={cn("flex items-center gap-2", className)}>;
    <Icon className={cn("h-4 w-4", iconClassName)} />;
    <span className="text-sm text-muted-foreground">{label}</span>;
    <span className="font-bold">{value}</span>;
    </div>;
    );

}

} // namespace elizaos
