#include "index.test.hpp"

namespace elizaos {
namespace generated_testing {

bool IndexTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void IndexTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json IndexTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos
