#include "module-loader.test.hpp"

namespace elizaos {
namespace generated_testing {

bool ModuleLoaderTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void ModuleLoaderTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json ModuleLoaderTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos
