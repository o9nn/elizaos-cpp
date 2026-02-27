#include "cryptoNews.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std: MaxTokens(const std:& data, double maxTokens = DEFAULT_MAX_CRYPTO_NEWS_CHARS) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return data.size() > maxTokens ? data.slice(0, maxTokens) : data;

}

} // namespace elizaos
