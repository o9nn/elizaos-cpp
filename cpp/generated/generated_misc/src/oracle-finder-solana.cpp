#include "oracle-finder-solana.hpp"

namespace elizaos {
namespace generated_misc {

bool OracleFinderSolana::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void OracleFinderSolana::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json OracleFinderSolana::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos
