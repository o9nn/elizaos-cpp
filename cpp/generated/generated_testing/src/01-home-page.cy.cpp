#include "01-home-page.cy.hpp"

namespace elizaos {
namespace generated_testing {

bool Test01HomePageCy::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Test01HomePageCy::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Test01HomePageCy::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos
