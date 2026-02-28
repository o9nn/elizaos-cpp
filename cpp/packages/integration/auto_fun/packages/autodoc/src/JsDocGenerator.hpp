#ifndef ELIZAOS_CPP_PACKAGES_INTEGRATION_AUTO_FUN_PACKAGES_AUTODOC_SRC_JSDOCGENERATOR_HPP_
#define ELIZAOS_CPP_PACKAGES_INTEGRATION_AUTO_FUN_PACKAGES_AUTODOC_SRC_JSDOCGENERATOR_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace autofun_autodoc {

class Jsdocgenerator {
public:
    Jsdocgenerator() = default;
    ~Jsdocgenerator() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "JsDocGenerator"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace autofun_autodoc
} // namespace elizaos

#endif // ELIZAOS_CPP_PACKAGES_INTEGRATION_AUTO_FUN_PACKAGES_AUTODOC_SRC_JSDOCGENERATOR_HPP_
