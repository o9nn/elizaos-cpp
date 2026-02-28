#ifndef ELIZAOS_CPP_PACKAGES_INTEGRATION_AUTO_FUN_PACKAGES_PROGRAM_TESTS_CHECKCONFIG_HPP_
#define ELIZAOS_CPP_PACKAGES_INTEGRATION_AUTO_FUN_PACKAGES_PROGRAM_TESTS_CHECKCONFIG_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace autofun_program {

class Checkconfig {
public:
    Checkconfig() = default;
    ~Checkconfig() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "checkConfig"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace autofun_program
} // namespace elizaos

#endif // ELIZAOS_CPP_PACKAGES_INTEGRATION_AUTO_FUN_PACKAGES_PROGRAM_TESTS_CHECKCONFIG_HPP_
