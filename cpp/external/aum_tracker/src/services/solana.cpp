#include "solana.hpp"

namespace elizaos {
namespace aum_tracker {

bool Solana::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Solana::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Solana::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace aum_tracker
} // namespace elizaos
