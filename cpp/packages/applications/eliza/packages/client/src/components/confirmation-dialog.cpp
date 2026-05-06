#include "confirmation-dialog.hpp"

namespace elizaos {
namespace eliza_client {

bool ConfirmationDialog::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void ConfirmationDialog::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json ConfirmationDialog::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos
