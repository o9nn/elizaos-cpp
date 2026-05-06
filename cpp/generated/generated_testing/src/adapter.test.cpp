#include "adapter.test.hpp"

namespace elizaos {
namespace generated_testing {

bool AdapterTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void AdapterTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json AdapterTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos
