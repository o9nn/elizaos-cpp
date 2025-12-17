#include "webFetch.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string MaxTokens(const std::string& data, number = DEFAULT_MAX_FETCH_CHARS maxTokens) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Character-based truncation to cap response length
    return data.length > maxTokens ? data.slice(0, maxTokens) : data;

}

} // namespace elizaos
