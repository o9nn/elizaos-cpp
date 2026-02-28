#ifndef ELIZAOS_CPP_GENERATED_GENERATED_CLI_INCLUDE_CLI_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_CLI_INCLUDE_CLI_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_cli {

class Cli {
public:
    Cli() = default;
    ~Cli() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "cli"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_cli
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_CLI_INCLUDE_CLI_HPP_
