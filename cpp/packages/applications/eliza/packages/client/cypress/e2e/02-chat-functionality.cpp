#include "02-chat-functionality.hpp"

namespace elizaos {
namespace elizaos_module {

bool Test02ChatFunctionality::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Test02ChatFunctionality::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Test02ChatFunctionality::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace elizaos_module
} // namespace elizaos
