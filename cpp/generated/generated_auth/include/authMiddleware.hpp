#ifndef ELIZAOS_CPP_GENERATED_GENERATED_AUTH_INCLUDE_AUTHMIDDLEWARE_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_AUTH_INCLUDE_AUTHMIDDLEWARE_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_auth {

class Authmiddleware {
public:
    Authmiddleware() = default;
    ~Authmiddleware() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "authMiddleware"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_auth
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_AUTH_INCLUDE_AUTHMIDDLEWARE_HPP_
