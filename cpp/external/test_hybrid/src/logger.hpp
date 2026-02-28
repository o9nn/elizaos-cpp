#ifndef ELIZAOS_CPP_EXTERNAL_TEST_HYBRID_SRC_LOGGER_HPP_
#define ELIZAOS_CPP_EXTERNAL_TEST_HYBRID_SRC_LOGGER_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace test_hybrid {

class Logger {
public:
    Logger() = default;
    ~Logger() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "logger"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace test_hybrid
} // namespace elizaos

#endif // ELIZAOS_CPP_EXTERNAL_TEST_HYBRID_SRC_LOGGER_HPP_
