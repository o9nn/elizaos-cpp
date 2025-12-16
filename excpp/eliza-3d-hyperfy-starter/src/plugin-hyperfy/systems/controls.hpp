#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".hyperfy/src/core/extras/Vector3Enhanced.js.hpp"
#include ".hyperfy/src/core/systems/System.js.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Define Navigation Constants

void createButtonState();

class ControlsToken {
public:
    void abort();
    void aborted() const;
};

class AgentControls {
public:
    AgentControls(const std::any& world);
    void setKey(const std::string& keyName, bool isDown);
    void postLateUpdate();
    void startRandomWalk(number = RANDOM_WALK_DEFAULT_INTERVAL interval, number = RANDOM_WALK_DEFAULT_MAX_DISTANCE maxDistance, number = 30000 duration);
    std::future<void> followEntity(const std::string& entityId, number = FOLLOW_STOP_DISTANCE stopDistance);
    std::future<void> goto(double x, double z);
    std::future<void> rotateTo(const std::variant<'front', 'back', 'left', 'right'>& direction, number = 500 duration);
    void stopRandomWalk();
    void stopNavigation(string = "commanded" reason);
    void stopRotation();
    void stopAllActions(string = "stopAllActions called" reason);
    bool getIsNavigating();
    bool getIsWalkingRandomly();
    bool _validatePlayerState(const std::string& caller);
    void createCamera(auto self);
    void bind(const std::any& options);
    void release();
    void setActions();

private:
    std::any keyW_;
    std::any keyA_;
    std::any keyS_;
    std::any keyD_;
    std::any space_;
    std::any shiftLeft_;
    std::any shiftRight_;
    std::any controlLeft_;
    std::any keyC_;
    std::any keyF_;
    std::any keyE_;
    std::any arrowUp_;
    std::any arrowDown_;
    std::any arrowLeft_;
    std::any arrowRight_;
    std::any touchA_;
    std::any touchB_;
    std::any xrLeftBtn1_;
    std::any xrLeftBtn2_;
    std::any xrRightBtn1_;
    std::any xrRightBtn2_;
};


} // namespace elizaos
