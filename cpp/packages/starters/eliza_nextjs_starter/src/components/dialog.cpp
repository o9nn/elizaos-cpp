#include "dialog.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Dialog(auto className, auto children, auto variant) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <Headless.Dialog {...props}>;
    {variant == "modal" && (;
    <Headless.DialogBackdrop;
    transition;
    className="fixed inset-0 flex w-screen justify-center overflow-y-auto bg-zinc-950/25 px-2 py-2 transition duration-100 focus:outline-0 data-[closed]:opacity-0 data-[enter]:ease-out data-[leave]:ease-in sm:px-6 sm:py-8 lg:px-8 lg:py-16 dark:bg-zinc-950/50"
    />;
    )}

    {variant == "modal" ? (;
    <div className="fixed inset-0 w-screen overflow-y-auto pt-6 sm:pt-0">
    <div className="grid min-h-full grid-rows-[1fr_auto] justify-items-center sm:grid-rows-[1fr_auto_3fr] sm:p-4">
    <Headless.DialogPanel;
    transition;
    className={clsx(;
    className,
    sizes[size],
    "row-start-2 w-full min-w-0 rounded-t-3xl bg-white p-[--gutter] shadow-lg ring-1 ring-zinc-950/10 [--gutter:theme(spacing.8)] sm:mb-auto sm:rounded-2xl dark:bg-zinc-900 dark:ring-white/10 forced-colors:outline",
    "transition duration-100 will-change-transform data-[closed]:translate-y-12 data-[closed]:opacity-0 data-[enter]:ease-out data-[leave]:ease-in sm:data-[closed]:translate-y-0 sm:data-[closed]:data-[enter]:scale-95",
    )}
    >;
    {children}
    </Headless.DialogPanel>;
    </div>;
    </div>;
    ) : (
    <Headless.DialogPanel;
    transition;
    className={clsx(;
    className,
    "fixed inset-y-0 right-0 z-50 w-full overflow-y-auto bg-white dark:bg-black transition duration-300 ease-in-out data-[closed]:translate-x-full",
    "sm:max-w-sm",
    )}
    >;
    {children}
    </Headless.DialogPanel>;
    )}
    </Headless.Dialog>;
    );

}

void DialogTitle() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <Headless.DialogTitle;
    {...props}
    className={clsx(;
    className,
    "text-balance text-lg/6 font-semibold text-zinc-950 sm:text-base/6 dark:text-white",
    )}
    />;
    );

}

void DialogDescription() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <Headless.Description;
    as={Text}
    {...props}
    className={clsx(className, "mt-2 text-pretty")}
    />;
    );

}

void DialogBody() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return <div {...props} className={clsx(className, "mt-6")} />;

}

void DialogActions() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div;
    {...props}
    className={clsx(;
    className,
    "mt-8 flex flex-col-reverse items-center justify-end gap-3 *:w-full sm:flex-row sm:*:w-auto",
    )}
    />;
    );

}

} // namespace elizaos
