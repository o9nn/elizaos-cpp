#include "installPluginFromRegistry.test.hpp"

namespace elizaos {
namespace generated_testing {

bool InstallpluginfromregistryTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void InstallpluginfromregistryTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json InstallpluginfromregistryTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos
