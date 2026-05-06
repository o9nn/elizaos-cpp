#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using SidebarContext = std::variant<{

void useSidebar();

    // This is the internal state of the sidebar.
    // We use openProp and setOpenProp for control from outside the component.

        // This sets the cookie to keep the sidebar state.

    // Helper to toggle the sidebar.

    // Adds a keyboard shortcut to toggle the sidebar.

    // We add a state so that we can do data-state="expanded" or "collapsed".
    // This makes it easier to style the sidebar with Tailwind classes.

            // Adjust the padding for floating and inset variants.

        // Increases the hit area of the button on mobile.

        // Increases the hit area of the button on mobile.

  // Random width between 50 to 90%.


} // namespace elizaos
