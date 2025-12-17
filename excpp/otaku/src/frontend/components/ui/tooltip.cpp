#include "tooltip.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void TooltipProvider() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <TooltipPrimitive.Provider;
    data-slot="tooltip-provider";
    delayDuration={delayDuration}
    {...props}
    />;
    );

}

void Tooltip() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <TooltipProvider>;
    <TooltipPrimitive.Root data-slot="tooltip" {...props} />;
    </TooltipProvider>;
    );

}

void TooltipTrigger() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return <TooltipPrimitive.Trigger data-slot="tooltip-trigger" {...props} />;

}

void TooltipContent(auto sideOffset, auto children) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <TooltipPrimitive.Portal>;
    <TooltipPrimitive.Content;
    data-slot="tooltip-content";
    sideOffset={sideOffset}
    className={cn(;
    "bg-primary text-primary-foreground animate-in fade-in-0 zoom-in-95 data-[state=closed]:animate-out data-[state=closed]:fade-out-0 data-[state=closed]:zoom-out-95 data-[side=bottom]:slide-in-from-top-2 data-[side=left]:slide-in-from-right-2 data-[side=right]:slide-in-from-left-2 data-[side=top]:slide-in-from-bottom-2 z-50 w-fit origin-(--radix-tooltip-content-transform-origin) rounded-md px-3 py-1.5 text-xs text-balance",
    className;
    )}
    {...props}
    >;
    {children}
    <TooltipPrimitive.Arrow className="bg-primary fill-primary z-50 size-2.5 translate-y-[calc(-50%_-_2px)] rotate-45 rounded-[2px]" />;
    </TooltipPrimitive.Content>;
    </TooltipPrimitive.Portal>;
    );

}

} // namespace elizaos
