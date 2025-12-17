#include "text.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Text(auto {
  className, ComponentPropsWithoutRef<"p"> ...props
}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <p;
    data-slot="text";
    {...props}
    className={clsx(;
    className,
    "text-base/6 text-zinc-500 sm:text-sm/6 dark:text-zinc-400",
    )}
    />;
    );

}

void TextLink(auto {
  className, ComponentPropsWithoutRef<typeof Link> ...props
}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <Link;
    {...props}
    className={clsx(;
    className,
    "text-zinc-950 underline decoration-zinc-950/50 data-[hover]:decoration-zinc-950 dark:text-white dark:decoration-white/50 dark:data-[hover]:decoration-white",
    )}
    />;
    );

}

void Strong(auto {
  className, ComponentPropsWithoutRef<"strong"> ...props
}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <strong;
    {...props}
    className={clsx(className, "font-medium text-zinc-950 dark:text-white")}
    />;
    );

}

void Code(auto {
  className, ComponentPropsWithoutRef<"code"> ...props
}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <code;
    {...props}
    className={clsx(;
    className,
    "rounded border border-zinc-950/10 bg-zinc-950/[2.5%] px-0.5 text-sm font-medium text-zinc-950 sm:text-[0.8125rem] dark:border-white/20 dark:bg-white/5 dark:text-white",
    )}
    />;
    );

}

} // namespace elizaos
