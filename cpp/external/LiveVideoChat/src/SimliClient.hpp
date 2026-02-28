#ifndef ELIZAOS_CPP_EXTERNAL_LIVEVIDEOCHAT_SRC_SIMLICLIENT_HPP_
#define ELIZAOS_CPP_EXTERNAL_LIVEVIDEOCHAT_SRC_SIMLICLIENT_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace LiveVideoChat {

class Simliclient {
public:
    Simliclient() = default;
    ~Simliclient() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "SimliClient"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace LiveVideoChat
} // namespace elizaos

#endif // ELIZAOS_CPP_EXTERNAL_LIVEVIDEOCHAT_SRC_SIMLICLIENT_HPP_
