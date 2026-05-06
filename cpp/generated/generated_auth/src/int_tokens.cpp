#include "int_tokens.hpp"

namespace elizaos {
namespace generated_auth {

bool IntTokens::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void IntTokens::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json IntTokens::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_auth
} // namespace elizaos
