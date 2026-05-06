#include "create.test.hpp"

namespace elizaos {
namespace generated_testing {

bool CreateTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void CreateTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json CreateTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos
