#include "tradeMemoryService.hpp"

namespace elizaos {
namespace generated_services {

bool Tradememoryservice::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Tradememoryservice::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Tradememoryservice::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_services
} // namespace elizaos
