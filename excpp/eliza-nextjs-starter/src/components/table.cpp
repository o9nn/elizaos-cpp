#include "table.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Table(auto {
  bleed = false, auto dense = false, auto grid = false, auto striped = false, auto className, auto children, std::optional<{
  bleed: boolean;
  dense: boolean;
  grid: boolean;
  striped: boolean;
} & React::ComponentPropsWithoutRef<"div">> ...props
}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <TableContext.Provider;
    value={
        { bleed, dense, grid, striped }.ContextType<;
        typeof TableContext;
        >;
    }
    >;
    <div className="flow-root">;
    <div;
    {...props}
    className={clsx(;
    className,
    "-mx-[--gutter] overflow-x-auto whitespace-nowrap",
    )}
    >;
    <div;
    className={clsx(;
    "inline-block min-w-full align-middle",
    !bleed && "sm:px-[--gutter]",
    )}
    >;
    <table className="min-w-full text-left text-sm/6 text-zinc-950 dark:text-white">
    {children}
    </table>;
    </div>;
    </div>;
    </div>;
    </TableContext.Provider>;
    );

}

void TableHead(auto {
  className, ComponentPropsWithoutRef<"thead"> ...props
}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <thead;
    {...props}
    className={clsx(className, "text-zinc-500 dark:text-zinc-400")}
    />;
    );

}

void TableBody(ComponentPropsWithoutRef<"tbody"> props) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return <tbody {...props} />;

}

void TableRow(auto {
  href, auto target, auto title, auto className, std::optional<{
  href: string;
  target: string;
  title: string;
} & React::ComponentPropsWithoutRef<"tr">> ...props
}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { striped } = useContext(TableContext);

    return (;
    <TableRowContext.Provider;
    value={
        { href, target, title }.ContextType<typeof TableRowContext>;
    }
    >;
    <tr;
    {...props}
    className={clsx(;
    className,
    href &&;
    "has-[[data-row-link][data-focus]]:outline has-[[data-row-link][data-focus]]:outline-2 has-[[data-row-link][data-focus]]:-outline-offset-2 has-[[data-row-link][data-focus]]:outline-blue-500 dark:focus-within:bg-white/[2.5%]",
    striped && "even:bg-zinc-950/[2.5%] dark:even:bg-white/[2.5%]",
    href && striped && "hover:bg-zinc-950/5 dark:hover:bg-white/5",
    href &&;
    !striped &&;
    "hover:bg-zinc-950/[2.5%] dark:hover:bg-white/[2.5%]",
    )}
    />;
    </TableRowContext.Provider>;
    );

}

void TableHeader(auto {
  className, ComponentPropsWithoutRef<"th"> ...props
}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { bleed, grid } = useContext(TableContext);

    return (;
    <th;
    {...props}
    className={clsx(;
    className,
    "border-b border-b-zinc-950/10 px-4 py-2 font-medium first:pl-[var(--gutter,theme(spacing.2))] last:pr-[var(--gutter,theme(spacing.2))] dark:border-b-white/10",
    grid &&;
    "border-l border-l-zinc-950/5 first:border-l-0 dark:border-l-white/5",
    !bleed && "sm:first:pl-1 sm:last:pr-1",
    )}
    />;
    );

}

void TableCell(auto {
  className, auto children, ComponentPropsWithoutRef<"td"> ...props
}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { bleed, dense, grid, striped } = useContext(TableContext);
    const auto { href, target, title } = useContext(TableRowContext);
    const auto [cellRef, setCellRef] = useState<HTMLElement | nullptr>(nullptr);

    return (;
    <td;
    ref={href ? setCellRef : std::nullopt}
    {...props}
    className={clsx(;
    className,
    "relative px-4 first:pl-[var(--gutter,theme(spacing.2))] last:pr-[var(--gutter,theme(spacing.2))]",
    !striped && "border-b border-zinc-950/5 dark:border-white/5",
    grid &&;
    "border-l border-l-zinc-950/5 first:border-l-0 dark:border-l-white/5",
    dense ? "py-2.5" : "py-4",
    !bleed && "sm:first:pl-1 sm:last:pr-1",
    )}
    >;
    {href && (;
    <Link;
    data-row-link;
    href={href}
    target={target}
    aria-label={title}
    tabIndex={cellRef.previousElementSibling == nullptr ? 0 : -1}
    className="absolute inset-0 focus:outline-none"
    />;
    )}
    {children}
    </td>;
    );

}

} // namespace elizaos
