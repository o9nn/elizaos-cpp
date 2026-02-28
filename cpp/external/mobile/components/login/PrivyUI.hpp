#ifndef ELIZAOS_CPP_EXTERNAL_MOBILE_COMPONENTS_LOGIN_PRIVYUI_HPP_
#define ELIZAOS_CPP_EXTERNAL_MOBILE_COMPONENTS_LOGIN_PRIVYUI_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace mobile {

class Privyui {
public:
    Privyui() = default;
    ~Privyui() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "PrivyUI"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace mobile
} // namespace elizaos

#endif // ELIZAOS_CPP_EXTERNAL_MOBILE_COMPONENTS_LOGIN_PRIVYUI_HPP_
