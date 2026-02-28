#include "01-home-page.hpp"

namespace elizaos {
namespace elizaos_module {

bool Test01HomePage::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Test01HomePage::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Test01HomePage::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace elizaos_module
} // namespace elizaos
