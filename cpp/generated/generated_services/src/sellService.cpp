#include "sellService.hpp"

namespace elizaos {
namespace generated_services {

bool Sellservice::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Sellservice::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Sellservice::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_services
} // namespace elizaos
