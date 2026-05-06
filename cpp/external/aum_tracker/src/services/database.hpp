#ifndef ELIZAOS_CPP_EXTERNAL_AUM_TRACKER_SRC_SERVICES_DATABASE_HPP_
#define ELIZAOS_CPP_EXTERNAL_AUM_TRACKER_SRC_SERVICES_DATABASE_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace aum_tracker {

class Database {
public:
    Database() = default;
    ~Database() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "database"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace aum_tracker
} // namespace elizaos

#endif // ELIZAOS_CPP_EXTERNAL_AUM_TRACKER_SRC_SERVICES_DATABASE_HPP_
