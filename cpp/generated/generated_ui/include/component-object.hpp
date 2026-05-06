#ifndef ELIZAOS_CPP_GENERATED_GENERATED_UI_INCLUDE_COMPONENT_OBJECT_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_UI_INCLUDE_COMPONENT_OBJECT_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_ui {

class ComponentObject {
public:
    ComponentObject() = default;
    ~ComponentObject() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "component_object"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_ui
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_UI_INCLUDE_COMPONENT_OBJECT_HPP_
