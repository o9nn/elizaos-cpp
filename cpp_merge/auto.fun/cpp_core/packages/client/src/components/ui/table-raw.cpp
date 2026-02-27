#include "table-raw.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Table() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div;
    data-slot="table-container";
    className="relative w-full overflow-x-auto border-t";
    >;
    <table;
    data-slot="table";
    className={twMerge("w-full caption-bottom text-sm", className)}
    {...props}
    />;
    </div>;
    );

}

void TableHeader() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <thead;
    data-slot="table-header";
    className={twMerge("[&_tr]:border-b", className)}
    {...props}
    />;
    );

}

void TableBody() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <tbody;
    data-slot="table-body";
    className={twMerge("[&_tr:last-child]:border-0", className)}
    {...props}
    />;
    );

}

void TableRow() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <tr;
    data-slot="table-row";
    className={twMerge(;
    "hover:bg-muted/50 data-[state=selected]:bg-muted border-b transition-colors",
    className,
    )}
    {...props}
    />;
    );

}

void TableHead() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <th;
    data-slot="table-head";
    className={twMerge(;
    "first:pl-4 last:pr-4 text-foreground font-dm-mono font-normal text-sm text-autofun-text-secondary uppercase tracking-[2px] h-10 px-2 text-left align-middle whitespace-nowrap [&:has([role=checkbox])]:pr-0 [&>[role=checkbox]]:translate-y-[2px]",
    className,
    )}
    {...props}
    />;
    );

}

void TableCell() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <td;
    data-slot="table-cell";
    className={twMerge(;
    "first:pl-4 last:pr-4 p-2 align-middle whitespace-nowrap [&:has([role=checkbox])]:pr-0 [&>[role=checkbox]]:translate-y-[2px] font-dm-mono text-base",
    className,
    )}
    {...props}
    />;
    );

}

} // namespace elizaos
