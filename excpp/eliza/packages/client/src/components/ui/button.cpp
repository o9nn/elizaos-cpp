#include "button.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Button(auto variant, auto size, auto asChild) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto Comp = asChild ? Slot : "button";

    return (;
    <Comp;
    data-slot="button";
    className={cn(buttonVariants({ variant, size, className }))}
    {...props}
    />;
    );

}

} // namespace elizaos
