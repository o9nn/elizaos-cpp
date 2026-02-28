#ifndef ELIZAOS_CPP_EXTERNAL_AUM_TRACKER_SRC_ROUTES_API_HPP_
#define ELIZAOS_CPP_EXTERNAL_AUM_TRACKER_SRC_ROUTES_API_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace aum_tracker {

class Api {
public:
    Api() = default;
    ~Api() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "api"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace aum_tracker
} // namespace elizaos

#endif // ELIZAOS_CPP_EXTERNAL_AUM_TRACKER_SRC_ROUTES_API_HPP_
