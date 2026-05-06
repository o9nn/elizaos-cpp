#include "token-search-symbol.hpp"

namespace elizaos {
namespace generated_auth {

bool TokenSearchSymbol::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void TokenSearchSymbol::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json TokenSearchSymbol::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_auth
} // namespace elizaos
