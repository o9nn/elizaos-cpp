#include "dialog.hpp"

namespace elizaos {
namespace autofun_client {

bool Dialog::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Dialog::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Dialog::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_client
} // namespace elizaos
