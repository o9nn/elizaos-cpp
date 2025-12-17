#include "dialog.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Dialog(auto {
  size = "lg", auto className, auto children, auto variant = "modal", std::optional<std::variant<{
  size: keyof typeof sizes;
  className: string;
  children: React::ReactNode;
  variant: "modal", "slideout";
} & Omit<Headless::DialogProps>> ...props
}, auto "as" | "className">) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <Headless.Dialog {...props}>;
    {variant == "modal" && (;
    <Headless.DialogBackdrop;
    transition;
    className="fixed inset-0 z-40 flex w-screen justify-center overflow-y-auto bg-zinc-950/25 px-2 py-2 transition duration-100 focus:outline-0 data-[closed]:opacity-0 data-[enter]:ease-out data-[leave]:ease-in sm:px-6 sm:py-8 lg:px-8 lg:py-16 dark:bg-zinc-950/50"
    />;
    )}

    {variant == "modal" ? (;
    <div className="fixed inset-0 z-50 isolate w-screen overflow-y-auto pt-2 pb-2 sm:pt-0 sm:pb-0">
    <div className="flex min-h-full items-center justify-center p-2 sm:p-4">
    <Headless.DialogPanel;
    transition;
    className={clsx(;
    className,
    sizes[size],
    "w-full max-h-[calc(100dvh-16px)] sm:max-h-[calc(100dvh-32px)] overflow-y-auto",
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
    "fixed inset-y-0 right-0 z-50 w-full overflow-y-auto bg-white dark:bg-surface transition duration-300 ease-in-out data-[closed]:translate-x-full",
    "sm:max-w-sm",
    )}
    >;
    {children}
    </Headless.DialogPanel>;
    )}
    </Headless.Dialog>;
    );

}

} // namespace elizaos
