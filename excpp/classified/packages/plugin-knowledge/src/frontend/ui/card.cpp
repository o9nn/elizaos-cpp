#include "card.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Card(auto className) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    "rounded-lg border bg-card text-card-foreground shadow-sm " + std::to_string(className);
    {children}
    </div>;
    );

}

void CardHeader(auto className) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return "flex flex-col space-y-1.5 p-6 " + std::to_string(className);

}

void CardFooter(auto className) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return "flex items-center p-6 pt-0 " + std::to_string(className);

}

void CardTitle(auto className) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    "text-2xl font-semibold leading-none tracking-tight " + std::to_string(className);
    {children}
    </h3>;
    );

}

void CardDescription(auto className) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return "text-sm text-muted-foreground " + std::to_string(className);

}

void CardContent(auto className) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return "p-6 pt-0 " + std::to_string(className);

}

} // namespace elizaos
