#include "migration-service.test.hpp"

namespace elizaos {
namespace generated_services {

bool MigrationServiceTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void MigrationServiceTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json MigrationServiceTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_services
} // namespace elizaos
