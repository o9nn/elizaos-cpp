#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".hyperfy/src/core/extras/appTools.js.hpp"
#include ".hyperfy/src/core/utils-client.js.hpp"
#include ".hyperfy/src/core/utils.js.hpp"
#include ".service.hpp"
#include ".utils.js.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class BuildManager {
public:
    BuildManager(IAgentRuntime runtime);
    void translate(auto entityId, [number position, auto number);
    void rotate(auto entityId, [number quaternion, auto number, auto number);
    void scale(auto entityId, [number scale, auto number);
    void duplicate(auto entityId);
    void delete(auto entityId);
    void importEntity(const std::string& url, std::optional<std::any> position, std::optional<std::any> quaternion);
    void addApp(auto file, auto transform);
    void addModel(auto file, auto transform);
    void entityUpdate(auto entity);
    void getService();

private:
    IAgentRuntime runtime_;
};


} // namespace elizaos
