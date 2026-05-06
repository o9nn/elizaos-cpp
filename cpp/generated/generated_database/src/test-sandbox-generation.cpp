#include "test-sandbox-generation.hpp"

namespace elizaos {
namespace generated_database {

bool TestSandboxGeneration::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void TestSandboxGeneration::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json TestSandboxGeneration::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_database
} // namespace elizaos
