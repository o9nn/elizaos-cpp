#include "button.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Button(auto outline, auto plain, auto className, auto children, auto type) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto classes = clsx(;
    className,
    styles.base,
    outline;
    ? styles.outline;
    : plain
    ? styles.plain;
    : clsx(styles.solid, styles.colors[color || "dark/zinc"]),
    );

    return "href" in props ? (;
    <Link {...props} className={classes}>;
    <TouchTarget>{children}</TouchTarget>;
    </Link>;
    ) : (
    <button;
    type={type}
    {...props}
    className={clsx(classes, "cursor-pointer disabled:cursor-default")}
    >;
    <TouchTarget>{children}</TouchTarget>;
    </button>;
    );

}

void TouchTarget() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <>;
    <span;
    className="absolute left-1/2 top-1/2 size-[max(100%,2.75rem)] -tranzinc-x-1/2 -tranzinc-y-1/2 [@media(pointer:fine)]:hidden"
    aria-hidden="true";
    />;
    {children}
    </>;
    );

}

} // namespace elizaos
