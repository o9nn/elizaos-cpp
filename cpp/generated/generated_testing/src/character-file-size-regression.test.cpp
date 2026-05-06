#include "character-file-size-regression.test.hpp"

namespace elizaos {
namespace generated_testing {

bool CharacterFileSizeRegressionTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void CharacterFileSizeRegressionTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json CharacterFileSizeRegressionTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos
