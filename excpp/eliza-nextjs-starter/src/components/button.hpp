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



    // Base
    // Sizing
    // Focus
    // Disabled
    // Icon
    // Optical border, implemented as the button background to avoid corner artifacts
    // Dark mode: border is rendered on `after` so background is set to button background
    // Button background, implemented as foreground layer to stack on top of pseudo-border layer
    // Drop shadow, applied to the inset `before` layer so it blends with the border
    // Background color is moved to control and shadow is removed in dark mode so hide `before` pseudo
    // Dark mode: Subtle white outline is applied using a border
    // Shim/overlay, inset to match button foreground and used for hover state + highlight shadow
    // Inner highlight shadow
    // White overlay on hover
    // Dark mode: `after` layer expands to cover entire button
    // Disabled
    // Base
    // Dark mode
    // Icon
    // Base
    // Dark mode
    // Icon

using ButtonProps = std::variant<(, { color?: keyof typeof styles.colors>; outline?: never; plain?: never }

/**
 * Expand the hit area to at least 44×44px on touch devices
 */
void TouchTarget({ children: React.ReactNode } { children });

} // namespace elizaos
