#include "xml-parser.test.hpp"

namespace elizaos {
namespace generated_testing {

bool XmlParserTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void XmlParserTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json XmlParserTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos
