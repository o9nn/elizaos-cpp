#include "posthog.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void PHProvider(const std::any& { children }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return <PostHogProvider client={posthog}>{children}</PostHogProvider>;

}

} // namespace elizaos
