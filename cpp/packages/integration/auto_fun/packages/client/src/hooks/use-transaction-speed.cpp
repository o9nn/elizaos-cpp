#include "use-transaction-speed.hpp"

namespace elizaos {
namespace autofun_client {

bool UseTransactionSpeed::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void UseTransactionSpeed::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json UseTransactionSpeed::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_client
} // namespace elizaos
