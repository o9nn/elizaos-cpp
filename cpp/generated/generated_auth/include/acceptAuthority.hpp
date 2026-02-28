#ifndef ELIZAOS_CPP_GENERATED_GENERATED_AUTH_INCLUDE_ACCEPTAUTHORITY_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_AUTH_INCLUDE_ACCEPTAUTHORITY_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_auth {

class Acceptauthority {
public:
    Acceptauthority() = default;
    ~Acceptauthority() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "acceptAuthority"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_auth
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_AUTH_INCLUDE_ACCEPTAUTHORITY_HPP_
