#include "stat-card.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void StatCard(Icon icon, auto bgColor, auto children, auto className, auto modalContent, auto modalTitle, auto modalDescription) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto HeaderContent = (;
    <div className="flex w-full items-center justify-between">;
    <div className="flex items-center gap-2">;
    <Icon className="h-4 w-4" /> {title}
    </div>;
    {modalContent && (;
    <ChevronRight className="h-4 w-4 text-muted-foreground" />;
    )}
    </div>;
    );

    const auto cardInnerContent = (;
    <>;
    <CardHeader className={cn(bgColor, "w-full py-4 transition-colors")}>;
    <CardTitle className="text-sm font-medium">{HeaderContent}</CardTitle>;
    </CardHeader>;
    <CardContent className="w-full flex-grow p-4">{children}</CardContent>;
    </>;
    );

    if (modalContent) {
        return (;
        <Dialog>;
        <DialogTrigger asChild>;
        <button;
        className={cn(;
        "group flex flex-col rounded-md border bg-card text-card-foreground shadow-sm",
        "ring-offset-background transition-colors focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-offset-2",
        className,
    )}
    >;
    {cardInnerContent}
    </button>;
    </DialogTrigger>;
    <DialogContent className="gap-0 p-0 sm:max-w-[525px]">
    <DialogHeader className="flex justify-center border-b border-border px-6 py-4 text-start">;
    <DialogTitle>{modalTitle || title}</DialogTitle>;
    {modalDescription && (;
    <DialogDescription>{modalDescription}</DialogDescription>;
    )}
    </DialogHeader>;
    {modalContent}
    </DialogContent>;
    </Dialog>;
    );
    }

    return (;
    <Card className={cn("flex flex-col overflow-hidden", className)}>;
    {cardInnerContent}
    </Card>;
    );

}

} // namespace elizaos
