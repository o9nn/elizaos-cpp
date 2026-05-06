#include "preprocessContent.test.hpp"

namespace elizaos {
namespace generated_testing {

bool PreprocesscontentTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void PreprocesscontentTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json PreprocesscontentTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos
