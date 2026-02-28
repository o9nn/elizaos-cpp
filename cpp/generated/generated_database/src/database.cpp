#include "database.hpp"

namespace elizaos {
namespace generated_database {

bool Database::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Database::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Database::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_database
} // namespace elizaos
