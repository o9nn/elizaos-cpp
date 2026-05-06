#include "swapUtils.hpp"

namespace elizaos {
namespace autofun_client {

bool Swaputils::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Swaputils::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Swaputils::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_client
} // namespace elizaos
