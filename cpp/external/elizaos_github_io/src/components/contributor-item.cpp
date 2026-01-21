#include "contributor-item.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ContributorItem(auto href, auto stats, auto score, auto className, auto children) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div;
    className={cn(;
    "flex flex-col gap-3 rounded-lg border p-4 transition-colors hover:bg-accent/50",
    className,
    )}
    >;
    <Link href={href} className="">;
    <div className="flex gap-3">;
    <Avatar className="h-10 w-10">;
    <AvatarImage;
    "src={" + "https://github.com/" + username + ".png"
    alt={username}
    />;
    <AvatarFallback>{username[0].toUpperCase()}</AvatarFallback>;
    </Avatar>;
    <div>;
    <div className="flex items-start gap-2 pb-1">;
    <h4 className="font-medium">{username}</h4>;
    <StatBadge;
    labelStart={score.toFixed(0)}
    value="XP";
    variant="secondary";
    />;
    </div>;
    <div className="text-xs text-muted-foreground">{stats}</div>;
    </div>;
    </div>;
    </Link>;

    {children}
    </div>;
    );

}

} // namespace elizaos
