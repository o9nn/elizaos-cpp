#include "tabs.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Tabs() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <TabsPrimitive.Root;
    data-slot="tabs";
    className={cn("flex flex-col gap-2", className)}
    {...props}
    />;
    );

}

void TabsList() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <TabsPrimitive.List;
    data-slot="tabs-list";
    className={cn(;
    "bg-foreground/5 text-muted-foreground inline-flex h-9 w-fit items-center justify-center rounded-lg p-[3px]",
    className;
    )}
    {...props}
    />;
    );

}

void TabsTrigger() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <TabsPrimitive.Trigger;
    data-slot="tabs-trigger";
    className={cn(;
    "text-sm md:text-base data-[state=active]:bg-primary font-medium data-[state=active]:font-bold text-foreground/60 data-[state=active]:text-foreground focus-visible:border-ring focus-visible:ring-ring/50 focus-visible:outline-ring inline-flex h-[calc(100%-1px)] flex-1 items-center justify-center gap-1.5 rounded-md border border-transparent px-3 py-2 whitespace-nowrap transition-[background-color,color,box-shadow] duration-300 focus-visible:ring-[3px] focus-visible:outline-1 disabled:pointer-events-none disabled:opacity-50 [&_svg]:pointer-events-none [&_svg]:shrink-0 [&_svg:not([class*='size-'])]:size-4",
    className;
    )}
    {...props}
    />;
    );

}

void TabsContent() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <TabsPrimitive.Content;
    data-slot="tabs-content";
    className={cn("flex-1 outline-none", className)}
    {...props}
    />;
    );

}

} // namespace elizaos
