#include "base.hpp"

namespace elizaos {
namespace eliza_api_client {

bool Base::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Base::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Base::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_api_client
} // namespace elizaos
