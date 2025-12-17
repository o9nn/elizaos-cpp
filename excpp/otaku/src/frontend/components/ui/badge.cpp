#include "badge.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Badge(auto {
  className, auto variant, auto asChild = false, std::optional<ComponentProps<"span"> &
  VariantProps<typeof badgeVariants> & { asChild: boolean }> ...props
}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto Comp = asChild ? Slot : "span";

    return (;
    <Comp;
    data-slot="badge";
    className={cn(badgeVariants({ variant }), className)}
    {...props}
    />;
    );

}

} // namespace elizaos
