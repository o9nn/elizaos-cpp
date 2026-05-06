#include "component.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void mountWithProviders(ReactNode component, auto options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryClient = createTestQueryClient();

    const auto wrapped = React.createElement(TooltipProvider, {;
        children: React.createElement(
        QueryClientProvider,
        { client: queryClient },
        React.createElement(;
        BrowserRouter,
        {},
        // Add a div container for portals
        React.createElement(;
        "div",
        {
            id: "root",
            style: { width: "100%", height: "100%" },
            },
            component,
            // Portal container for Radix UI
            React.createElement("div", { id: "radix-portal" })
            );
            );
            ),
            });

            return mount(wrapped, options);

}

void mountWithRouter(ReactNode component, auto options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryClient = createTestQueryClient();

    const auto wrapped = React.createElement(TooltipProvider, {;
        children: React.createElement(
        QueryClientProvider,
        { client: queryClient },
        React.createElement(;
        BrowserRouter,
        {},
        // Add a div container for portals
        React.createElement(;
        "div",
        {
            id: "root",
            style: { width: "100%", height: "100%" },
            },
            component,
            // Portal container for Radix UI
            React.createElement("div", { id: "radix-portal" })
            );
            );
            ),
            });

            return mount(wrapped, options);

}

void mountRadix(ReactNode component, auto options) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto queryClient = createTestQueryClient();

    const auto wrapped = React.createElement(TooltipProvider, {;
        children: React.createElement(
        DirectionProvider,
        { dir: "ltr" },
        React.createElement(;
        QueryClientProvider,
        { client: queryClient },
        React.createElement(;
        BrowserRouter,
        {},
        // Add a div container for portals
        React.createElement(;
        "div",
        {
            id: "root",
            style: { width: "100%", height: "100%" },
            },
            component,
            // Portal container for Radix UI
            React.createElement("div", { id: "radix-portal" })
            );
            );
            );
            ),
            });

            return mount(wrapped, options);

}

} // namespace elizaos
