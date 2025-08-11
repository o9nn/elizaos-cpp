#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <map>
#include <chrono>
#include <functional>

namespace elizaos {

/**
 * @brief Represents a 3D position in the virtual world
 */
struct WorldPosition {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    
    WorldPosition() = default;
    WorldPosition(double x, double y, double z) : x(x), y(y), z(z) {}
    
    double distanceTo(const WorldPosition& other) const;
    WorldPosition interpolate(const WorldPosition& target, double t) const;
};

/**
 * @brief Represents a virtual environment/zone within the world
 */
struct WorldEnvironment {
    std::string id;
    std::string name;
    std::string description;
    std::string type; // "social", "trading", "creative", "learning", etc.
    WorldPosition center;
    double radius = 100.0;
    std::map<std::string, std::string> properties;
    std::chrono::system_clock::time_point createdAt;
    bool active = true;
    
    bool containsPosition(const WorldPosition& pos) const;
    double distanceFromCenter(const WorldPosition& pos) const;
};

/**
 * @brief Represents an agent's presence in the virtual world
 */
struct WorldAgent {
    std::string agentId;
    std::string name;
    std::string type; // "eliza", "trader", "community", etc.
    WorldPosition position;
    WorldPosition velocity;
    double interactionRadius = 10.0;
    std::string currentEnvironment;
    std::map<std::string, std::string> attributes;
    std::chrono::system_clock::time_point lastUpdate;
    bool online = true;
    
    bool canInteractWith(const WorldAgent& other) const;
    void moveTo(const WorldPosition& target, double speed);
    void teleportTo(const WorldPosition& target);
};

/**
 * @brief Represents an interaction between agents in the world
 */
struct WorldInteraction {
    std::string id;
    std::string initiatorId;
    std::string targetId;
    std::string type; // "chat", "trade", "collaboration", etc.
    WorldPosition location;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> metadata;
    bool completed = false;
};

/**
 * @brief Callback function for world events
 */
using WorldEventCallback = std::function<void(const std::string&, const std::string&)>;

/**
 * @brief Main class for managing the virtual world and agent environments
 */
class ElizasWorld {
public:
    ElizasWorld();
    ~ElizasWorld() = default;

    // Environment management
    bool addEnvironment(const WorldEnvironment& environment);
    bool removeEnvironment(const std::string& environmentId);
    std::optional<WorldEnvironment> getEnvironment(const std::string& environmentId) const;
    std::vector<WorldEnvironment> getAllEnvironments() const;
    std::vector<WorldEnvironment> getActiveEnvironments() const;
    bool updateEnvironment(const WorldEnvironment& environment);

    // Agent management
    bool addAgent(const WorldAgent& agent);
    bool removeAgent(const std::string& agentId);
    std::optional<WorldAgent> getAgent(const std::string& agentId) const;
    std::vector<WorldAgent> getAllAgents() const;
    std::vector<WorldAgent> getOnlineAgents() const;
    std::vector<WorldAgent> getAgentsInEnvironment(const std::string& environmentId) const;
    bool updateAgent(const WorldAgent& agent);

    // Spatial queries
    std::vector<WorldAgent> getAgentsNearPosition(const WorldPosition& pos, double radius) const;
    std::vector<WorldAgent> getAgentsInRadius(const std::string& agentId, double radius) const;
    std::vector<WorldEnvironment> getEnvironmentsNearPosition(const WorldPosition& pos) const;
    std::string findEnvironmentForPosition(const WorldPosition& pos) const;

    // Agent positioning and movement
    bool moveAgent(const std::string& agentId, const WorldPosition& target, double speed = 1.0);
    bool teleportAgent(const std::string& agentId, const WorldPosition& target);
    bool setAgentVelocity(const std::string& agentId, const WorldPosition& velocity);

    // Interaction system
    bool recordInteraction(const WorldInteraction& interaction);
    std::vector<WorldInteraction> getInteractionHistory(const std::string& agentId) const;
    std::vector<WorldInteraction> getInteractionsInArea(const WorldPosition& center, double radius) const;
    std::vector<WorldInteraction> getRecentInteractions(int limit = 100) const;

    // World simulation
    void update(double deltaTime);
    void processAgentMovements(double deltaTime);
    void detectProximityInteractions();
    void updateEnvironmentStates();

    // Event system
    void onAgentEnterEnvironment(WorldEventCallback callback);
    void onAgentLeaveEnvironment(WorldEventCallback callback);
    void onAgentInteraction(WorldEventCallback callback);
    void onEnvironmentChange(WorldEventCallback callback);

    // Statistics and analytics
    size_t getAgentCount() const;
    size_t getEnvironmentCount() const;
    size_t getInteractionCount() const;
    double getWorldActivity() const; // Activity metric based on recent interactions
    std::map<std::string, int> getEnvironmentPopulations() const;
    std::vector<std::string> getMostActiveAgents(int limit = 10) const;

    // Data persistence
    bool saveWorldState(const std::string& filePath) const;
    bool loadWorldState(const std::string& filePath);
    std::string exportWorldData() const;
    bool importWorldData(const std::string& data);

    // Configuration
    void setWorldBounds(const WorldPosition& min, const WorldPosition& max);
    void setSimulationSpeed(double speed);
    void setAutoUpdate(bool enabled, double interval = 0.1); // Auto-update every 100ms

private:
    std::vector<WorldEnvironment> environments_;
    std::vector<WorldAgent> agents_;
    std::vector<WorldInteraction> interactions_;
    
    WorldPosition worldMin_;
    WorldPosition worldMax_;
    double simulationSpeed_;
    bool autoUpdateEnabled_;
    double updateInterval_;
    std::chrono::system_clock::time_point lastUpdate_;

    // Event callbacks
    WorldEventCallback onAgentEnterEnvCallback_;
    WorldEventCallback onAgentLeaveEnvCallback_;
    WorldEventCallback onInteractionCallback_;
    WorldEventCallback onEnvironmentChangeCallback_;

    // Helper methods
    std::vector<WorldAgent>::iterator findAgent(const std::string& agentId);
    std::vector<WorldAgent>::const_iterator findAgent(const std::string& agentId) const;
    std::vector<WorldEnvironment>::iterator findEnvironment(const std::string& environmentId);
    std::vector<WorldEnvironment>::const_iterator findEnvironment(const std::string& environmentId) const;
    
    void triggerAgentEnvironmentChange(const std::string& agentId, 
                                     const std::string& oldEnv, 
                                     const std::string& newEnv);
    void checkEnvironmentTransitions();
    bool isWithinWorldBounds(const WorldPosition& pos) const;
    std::string generateInteractionId() const;
};

} // namespace elizaos