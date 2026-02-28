#include "migration.test.hpp"

namespace elizaos {
namespace generated_testing {

bool MigrationTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void MigrationTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json MigrationTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos
