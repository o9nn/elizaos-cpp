#include "cryptoNews.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string MaxTokens(const std::string& data, double maxTokens = DEFAULT_MAX_CRYPTO_NEWS_CHARS) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return data.length > maxTokens ? data.slice(0, maxTokens) : data;

}

} // namespace elizaos
