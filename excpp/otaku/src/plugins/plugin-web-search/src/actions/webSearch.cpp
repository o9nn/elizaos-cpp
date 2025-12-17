#include "webSearch.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string MaxTokens(const std::string& data, double maxTokens = DEFAULT_MAX_WEB_SEARCH_CHARS) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Character-based truncation to cap response length
    return data.length > maxTokens ? data.slice(0, maxTokens) : data;

}

} // namespace elizaos
