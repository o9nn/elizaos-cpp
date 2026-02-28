#include "LinkAccounts.hpp"

namespace elizaos {
namespace mobile {

bool Linkaccounts::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Linkaccounts::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Linkaccounts::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace mobile
} // namespace elizaos
