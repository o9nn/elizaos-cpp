#include "fieldset.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Fieldset() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <Headless.Fieldset;
    {...props}
    className={clsx(;
    className,
    "[&>*+[data-slot=control]]:mt-6 [&>[data-slot=text]]:mt-1",
    )}
    />;
    );

}

void Legend() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <Headless.Legend;
    data-slot="legend";
    {...props}
    className={clsx(;
    className,
    "text-base/6 font-semibold text-zinc-950 data-[disabled]:opacity-50 sm:text-sm/6 dark:text-white",
    )}
    />;
    );

}

void FieldGroup() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div;
    data-slot="control";
    {...props}
    className={clsx(className, "space-y-8")}
    />;
    );

}

void Field() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <Headless.Field;
    {...props}
    className={clsx(;
    className,
    "[&>[data-slot=label]+[data-slot=control]]:mt-3",
    "[&>[data-slot=label]+[data-slot=description]]:mt-1",
    "[&>[data-slot=description]+[data-slot=control]]:mt-3",
    "[&>[data-slot=control]+[data-slot=description]]:mt-3",
    "[&>[data-slot=control]+[data-slot=error]]:mt-3",
    "[&>[data-slot=label]]:font-medium",
    )}
    />;
    );

}

void Label() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <Headless.Label;
    data-slot="label";
    {...props}
    className={clsx(;
    className,
    "select-none text-base/6 text-zinc-950 data-[disabled]:opacity-50 sm:text-sm/6 dark:text-white",
    )}
    />;
    );

}

void Description() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <Headless.Description;
    data-slot="description";
    {...props}
    className={clsx(;
    className,
    "text-base/6 text-zinc-500 data-[disabled]:opacity-50 sm:text-sm/6 dark:text-zinc-400",
    )}
    />;
    );

}

void ErrorMessage() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <Headless.Description;
    data-slot="error";
    {...props}
    className={clsx(;
    className,
    "text-base/6 text-red-600 data-[disabled]:opacity-50 sm:text-sm/6 dark:text-red-500",
    )}
    />;
    );

}

} // namespace elizaos
