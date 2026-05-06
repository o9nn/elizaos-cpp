#include "form.hpp"

namespace elizaos {
namespace autofun_client {

bool Form::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Form::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Form::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_client
} // namespace elizaos
