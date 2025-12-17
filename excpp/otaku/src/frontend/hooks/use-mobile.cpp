#include "use-mobile.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useIsMobile() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [isMobile, setIsMobile] = React.useState<boolean | std::nullopt>(std::nullopt);

    React.useEffect(() => {
        const auto mql = "window.matchMedia(" + "(max-width: " + std::to_string(MOBILE_BREAKPOINT - 1) + "px)";
        const auto onChange = [&]() {;
            setIsMobile(window.innerWidth < MOBILE_BREAKPOINT);
        }
        mql.addEventListener("change", onChange);
        setIsMobile(window.innerWidth < MOBILE_BREAKPOINT);
        return [&]() { return mql.removeEventListener("change", onChange); };
        }, []);

        return !!isMobile;

}

} // namespace elizaos
