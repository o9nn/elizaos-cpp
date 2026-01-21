#include "quoteService.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<QuoteService> getQuoteService(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return runtime.getService<QuoteService>("QuoteService");

}

} // namespace elizaos
