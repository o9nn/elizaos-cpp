#include "plugin-init.test.hpp"

namespace elizaos {
namespace generated_testing {

bool PluginInitTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void PluginInitTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json PluginInitTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos
