#include "test-fixtures.hpp"

namespace elizaos {
namespace generated_misc {

bool TestFixtures::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void TestFixtures::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json TestFixtures::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos
