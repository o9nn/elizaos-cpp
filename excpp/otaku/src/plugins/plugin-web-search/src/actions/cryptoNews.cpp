#include "cryptoNews.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string MaxTokens(const std::string& data, number = DEFAULT_MAX_CRYPTO_NEWS_CHARS maxTokens) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return data.length > maxTokens ? data.slice(0, maxTokens) : data;

}

} // namespace elizaos
