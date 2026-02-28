#include "hello-world.cy.hpp"

namespace elizaos {
namespace generated_testing {

bool HelloWorldCy::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void HelloWorldCy::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json HelloWorldCy::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos
