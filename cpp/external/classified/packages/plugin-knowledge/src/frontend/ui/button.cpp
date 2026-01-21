#include "button.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Button(auto variant, auto size, auto className, auto onClick, auto disabled, auto title, auto type) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto baseClasses =;
    "inline-flex items-center justify-center rounded-md font-medium transition-colors focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-offset-2 disabled:opacity-50 disabled:pointer-events-none";

    const auto variantClasses = {;
        default: "bg-primary text-primary-foreground hover:bg-primary/90",
        outline: "border border-input bg-background hover:bg-accent hover:text-accent-foreground",
        ghost: "hover:bg-accent hover:text-accent-foreground",
        destructive: "bg-destructive text-destructive-foreground hover:bg-destructive/90",
        };

        const auto sizeClasses = {;
            default: "h-10 px-4 py-2 text-sm",
            sm: "h-9 rounded-md px-3 text-xs",
            lg: "h-11 rounded-md px-8 text-sm",
            icon: "h-10 w-10 text-xs",
            };

            return (;
            <button;
        type={type}
    "className={" + baseClasses + " " + std::to_string(variantClasses[variant]) + " " + std::to_string(sizeClasses[size]) + " " + className;
    onClick={onClick}
    disabled={disabled}
    title={title}
    >;
    {children}
    </button>;
    );

}

} // namespace elizaos
