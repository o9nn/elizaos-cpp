#include "agent-export-import-simple.test.hpp"

namespace elizaos {
namespace generated_testing {

bool AgentExportImportSimpleTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void AgentExportImportSimpleTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json AgentExportImportSimpleTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos
