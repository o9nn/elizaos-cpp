#include "fees.hpp"

namespace elizaos {
namespace autofun_client {

bool Fees::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Fees::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Fees::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_client
} // namespace elizaos
