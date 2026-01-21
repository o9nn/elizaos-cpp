#include "badge.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Badge(auto variant, auto className) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto baseClasses = "inline-flex items-center rounded-full px-2.5 py-0.5 text-xs font-medium";

    const auto variantClasses = {;
        default: "bg-primary text-primary-foreground",
        outline: "border border-input bg-background text-foreground",
        secondary: "bg-secondary text-secondary-foreground",
        destructive: "bg-destructive text-destructive-foreground",
        };

        return (;
        "<span className={" + baseClasses + " " + std::to_string(variantClasses[variant]) + " " + className;
        );

}

} // namespace elizaos
