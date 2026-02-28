#include "generate-coverage-report.hpp"

namespace elizaos {
namespace eliza_cli {

bool GenerateCoverageReport::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void GenerateCoverageReport::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json GenerateCoverageReport::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_cli
} // namespace elizaos
