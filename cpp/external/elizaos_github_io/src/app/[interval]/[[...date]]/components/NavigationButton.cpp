#include "NavigationButton.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void NavigationButton(auto direction, auto isVisible) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto Icon = direction == "prev" ? ChevronLeft : ChevronRight;
    const auto label = direction == "prev" ? "Previous" : "Next";

    return (;
    <>;
    {/* Desktop */}
    <Button;
    variant="outline";
    asChild;
    className={cn("hidden items-center sm:flex", !isVisible && "invisible")}
    >;
    <Link href={href}>;
    {direction == "prev" && <Icon className="mr-2 h-4 w-4" />}
    <span>{label}</span>;
    {direction == "next" && <Icon className="ml-2 h-4 w-4" />}
    </Link>;
    </Button>;

    {/* Mobile */}
    <Button;
    variant="outline";
    size="icon";
    asChild;
    className={cn("sm:hidden", !isVisible && "invisible")}
    >;
    <Link href={href}>;
    <Icon className="h-4 w-4" />;
    <span className="sr-only">{label}</span>;
    </Link>;
    </Button>;
    </>;
    );

}

} // namespace elizaos
