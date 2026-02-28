#include "character-updater.test.hpp"

namespace elizaos {
namespace generated_testing {

bool CharacterUpdaterTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void CharacterUpdaterTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json CharacterUpdaterTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos
