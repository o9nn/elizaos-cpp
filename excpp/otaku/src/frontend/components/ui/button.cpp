#include "button.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Button(auto {
  className, auto variant, auto size, auto asChild = false, std::optional<ComponentProps<"button"> &
  VariantProps<typeof buttonVariants> & {
    asChild: boolean;
  }> ...props
}) {
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
