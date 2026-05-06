#ifndef ELIZAOS_CPP_GENERATED_GENERATED_TESTING_INCLUDE_SIMPLE_API_TEST_CY_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_TESTING_INCLUDE_SIMPLE_API_TEST_CY_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_testing {

class SimpleApiTestCy {
public:
    SimpleApiTestCy() = default;
    ~SimpleApiTestCy() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "simple_api_test_cy"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_testing
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_TESTING_INCLUDE_SIMPLE_API_TEST_CY_HPP_
