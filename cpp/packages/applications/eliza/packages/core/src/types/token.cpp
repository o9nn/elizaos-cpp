#include "token.hpp"

namespace elizaos {
namespace eliza_core {

bool Token::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Token::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Token::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_core
} // namespace elizaos
