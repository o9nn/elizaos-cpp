#include "buyer.setup.hpp"

namespace elizaos {
namespace generated_misc {

bool BuyerSetup::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void BuyerSetup::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json BuyerSetup::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos
