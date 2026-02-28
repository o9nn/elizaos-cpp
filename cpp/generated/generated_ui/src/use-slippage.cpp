#include "use-slippage.hpp"

namespace elizaos {
namespace generated_ui {

bool UseSlippage::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void UseSlippage::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json UseSlippage::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_ui
} // namespace elizaos
