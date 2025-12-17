#include "theme-provider.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ThemeProvider(auto { children, ThemeProviderProps ...props }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return <NextThemesProvider {...props}>{children}</NextThemesProvider>;

}

} // namespace elizaos
