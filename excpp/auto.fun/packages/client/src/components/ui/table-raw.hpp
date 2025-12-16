#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;

: React.ComponentProps<"table">) {
  return (
    <div
      data-slot="table-container"
      className="relative w-full overflow-x-auto border-t"
    >
      <table
        data-slot="table"
        className={twMerge("w-full caption-bottom text-sm", className)}
        {...props}
      />
    </div>
  );
}

: React.ComponentProps<"thead">) {
  return (
    <thead
      data-slot="table-header"
      className={twMerge("[&_tr]:border-b", className)}
      {...props}
    />
  );
}

: React.ComponentProps<"tbody">) {
  return (
    <tbody
      data-slot="table-body"
      className={twMerge("[&_tr:last-child]:border-0", className)}
      {...props}
    />
  );
}

: React.ComponentProps<"tr">) {
  return (
    <tr
      data-slot="table-row"
      className={twMerge(
        "hover:bg-muted/50 data-[state=selected]:bg-muted border-b transition-colors",
        className,
      )}
      {...props}
    />
  );
}

: React.ComponentProps<"th">) {
  return (
    <th
      data-slot="table-head"
      className={twMerge(
        "first:pl-4 last:pr-4 text-foreground font-dm-mono font-normal text-sm text-autofun-text-secondary uppercase tracking-[2px] h-10 px-2 text-left align-middle whitespace-nowrap [&:has([role=checkbox])]:pr-0 [&>[role=checkbox]]:translate-y-[2px]",
        className,
      )}
      {...props}
    />
  );
}

: React.ComponentProps<"td">) {
  return (
    <td
      data-slot="table-cell"
      className={twMerge(
        "first:pl-4 last:pr-4 p-2 align-middle whitespace-nowrap [&:has([role=checkbox])]:pr-0 [&>[role=checkbox]]:translate-y-[2px] font-dm-mono text-base",
        className,
      )}
      {...props}
    />
  );
}

{ Table, TableBody, TableCell, TableHead, TableHeader, TableRow };

} // namespace elizaos
