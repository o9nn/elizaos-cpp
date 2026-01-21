#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "hyperfy/src/core/systems/System.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class AgentActions {
public:
    AgentActions(const std::any& world);
    void register(ActionNode node);
    void unregister(ActionNode node);
    std::vector<ActionNode> getNearby(std::optional<double> maxDistance);
    void performAction(std::optional<std::string> entityID);
    void releaseAction();
    void init();
    void start();
    void preTick();
    void preFixedUpdate();
    void fixedUpdate();
    void postFixedUpdate();
    void preUpdate();
    void update();
    void postUpdate();
    void lateUpdate();
    void postLateUpdate();
    void commit();
    void postTick();
};

} // namespace elizaos
