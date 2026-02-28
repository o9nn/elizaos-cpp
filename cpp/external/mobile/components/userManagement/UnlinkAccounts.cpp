#include "UnlinkAccounts.hpp"

namespace elizaos {
namespace mobile {

bool Unlinkaccounts::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Unlinkaccounts::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Unlinkaccounts::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace mobile
} // namespace elizaos
