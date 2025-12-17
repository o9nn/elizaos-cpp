#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Simple autonomous loop service that can be toggled on/off via API
 * Continuously triggers agent thinking in a separate autonomous context
 */
class AutonomyService {
public:
    AutonomyService(IAgentRuntime runtime);
    std::future<AutonomyService> start(IAgentRuntime runtime);
    std::future<void> initialize();
    void setupSettingsMonitoring();
    std::future<void> startLoop();
    std::future<void> stopLoop();
    void scheduleNextThink();
    std::future<void> performAutonomousThink();
    std::string createMonologuePrompt(const std::string& lastThought, bool isFirstThought);
    std::future<void> broadcastThoughtToMonologue(const std::string& thoughtText, const std::string& messageId);
    bool isLoopRunning();
    double getLoopInterval();
    void setLoopInterval(double ms);
    UUID getAutonomousRoomId();
    std::future<void> enableAutonomy();
    std::future<void> disableAutonomy();
     getStatus();
    std::future<void> stop();
    std::string capabilityDescription() const;

private:
    UUID autonomousRoomId_;
    UUID autonomousWorldId_;
};


} // namespace elizaos
