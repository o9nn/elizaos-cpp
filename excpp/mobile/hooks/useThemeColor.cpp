#include "useThemeColor.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useThemeColor(std::optional<std::any> props, keyof typeof Colors::light colorName) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto theme = useColorScheme() || 'light';
    const auto colorFromProps = props[theme];

    if (colorFromProps) {
        return colorFromProps;
        } else {
            return Colors[theme][colorName];
        }

}

} // namespace elizaos
