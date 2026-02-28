#ifndef ELIZAOS_CPP_GENERATED_GENERATED_TESTING_INCLUDE_LOADER_TEST_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_TESTING_INCLUDE_LOADER_TEST_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_testing {

class LoaderTest {
public:
    LoaderTest() = default;
    ~LoaderTest() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "loader_test"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_testing
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_TESTING_INCLUDE_LOADER_TEST_HPP_
