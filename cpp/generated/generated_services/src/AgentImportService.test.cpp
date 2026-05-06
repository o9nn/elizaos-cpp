#include "AgentImportService.test.hpp"

namespace elizaos {
namespace generated_services {

bool AgentimportserviceTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void AgentimportserviceTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json AgentimportserviceTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_services
} // namespace elizaos
