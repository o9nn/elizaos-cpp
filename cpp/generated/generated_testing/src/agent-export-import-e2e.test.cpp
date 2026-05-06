#include "agent-export-import-e2e.test.hpp"

namespace elizaos {
namespace generated_testing {

bool AgentExportImportE2eTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void AgentExportImportE2eTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json AgentExportImportE2eTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos
