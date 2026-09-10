#include "elizaos/elizas_world.hpp"
#include <nlohmann/json.hpp>
#include <atomic>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <set>
#include <sstream>
#include <iomanip>
#include <random>
#include <utility>

namespace {

using json = nlohmann::json;
using elizaos::WorldAgent;
using elizaos::WorldEnvironment;
using elizaos::WorldInteraction;
using elizaos::WorldPosition;

constexpr int kWorldDataVersion = 1;
constexpr const char* kWorldDataFormat = "elizaos.elizas_world";

struct ParsedWorldState {
    std::vector<WorldEnvironment> environments;
    std::vector<WorldAgent> agents;
    std::vector<WorldInteraction> interactions;
    WorldPosition worldMin;
    WorldPosition worldMax;
    double simulationSpeed = 0.0;
    bool autoUpdateEnabled = false;
    double updateInterval = 0.0;
    std::chrono::system_clock::time_point lastUpdate;
};

json positionToJson(const WorldPosition& position) {
    return {
        {"x", position.x},
        {"y", position.y},
        {"z", position.z}
    };
}

std::int64_t timePointToNanoseconds(
    const std::chrono::system_clock::time_point& timePoint) {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               timePoint.time_since_epoch()).count();
}

bool parseString(const json& object, const char* key, std::string& value) {
    const auto& element = object.at(key);
    if (!element.is_string()) {
        return false;
    }
    value = element.get<std::string>();
    return true;
}

bool parseBoolean(const json& object, const char* key, bool& value) {
    const auto& element = object.at(key);
    if (!element.is_boolean()) {
        return false;
    }
    value = element.get<bool>();
    return true;
}

bool parseFiniteNumber(const json& object, const char* key, double& value) {
    const auto& element = object.at(key);
    if (!element.is_number()) {
        return false;
    }
    value = element.get<double>();
    return std::isfinite(value);
}

bool parsePosition(const json& value, WorldPosition& position) {
    if (!value.is_object()) {
        return false;
    }
    return parseFiniteNumber(value, "x", position.x) &&
           parseFiniteNumber(value, "y", position.y) &&
           parseFiniteNumber(value, "z", position.z);
}

bool parseStringMap(const json& value,
                    std::map<std::string, std::string>& result) {
    if (!value.is_object()) {
        return false;
    }

    std::map<std::string, std::string> parsed;
    for (auto it = value.begin(); it != value.end(); ++it) {
        if (!it.value().is_string()) {
            return false;
        }
        parsed.emplace(it.key(), it.value().get<std::string>());
    }
    result.swap(parsed);
    return true;
}

bool parseTimestamp(const json& object, const char* key,
                    std::chrono::system_clock::time_point& result) {
    const auto& value = object.at(key);
    std::int64_t nanosecondsSinceEpoch = 0;
    if (value.is_number_unsigned()) {
        const auto unsignedValue = value.get<std::uint64_t>();
        if (unsignedValue >
            static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max())) {
            return false;
        }
        nanosecondsSinceEpoch = static_cast<std::int64_t>(unsignedValue);
    } else if (value.is_number_integer()) {
        nanosecondsSinceEpoch = value.get<std::int64_t>();
    } else {
        return false;
    }

    const auto nanoseconds = std::chrono::nanoseconds(nanosecondsSinceEpoch);
    const auto clockDuration =
        std::chrono::duration_cast<std::chrono::system_clock::duration>(nanoseconds);
    if (std::chrono::duration_cast<std::chrono::nanoseconds>(clockDuration).count() !=
        nanosecondsSinceEpoch) {
        return false;
    }
    result = std::chrono::system_clock::time_point(clockDuration);
    return true;
}

bool isWithinBounds(const WorldPosition& position,
                    const WorldPosition& minimum,
                    const WorldPosition& maximum) {
    return position.x >= minimum.x && position.x <= maximum.x &&
           position.y >= minimum.y && position.y <= maximum.y &&
           position.z >= minimum.z && position.z <= maximum.z;
}

json environmentToJson(const WorldEnvironment& environment) {
    return {
        {"active", environment.active},
        {"center", positionToJson(environment.center)},
        {"created_at_ns", timePointToNanoseconds(environment.createdAt)},
        {"description", environment.description},
        {"id", environment.id},
        {"name", environment.name},
        {"properties", environment.properties},
        {"radius", environment.radius},
        {"type", environment.type}
    };
}

json agentToJson(const WorldAgent& agent) {
    return {
        {"agent_id", agent.agentId},
        {"attributes", agent.attributes},
        {"current_environment", agent.currentEnvironment},
        {"interaction_radius", agent.interactionRadius},
        {"last_update_ns", timePointToNanoseconds(agent.lastUpdate)},
        {"name", agent.name},
        {"online", agent.online},
        {"position", positionToJson(agent.position)},
        {"type", agent.type},
        {"velocity", positionToJson(agent.velocity)}
    };
}

json interactionToJson(const WorldInteraction& interaction) {
    return {
        {"completed", interaction.completed},
        {"id", interaction.id},
        {"initiator_id", interaction.initiatorId},
        {"location", positionToJson(interaction.location)},
        {"metadata", interaction.metadata},
        {"target_id", interaction.targetId},
        {"timestamp_ns", timePointToNanoseconds(interaction.timestamp)},
        {"type", interaction.type}
    };
}

bool parseWorldDocument(const json& root, ParsedWorldState& result) {
    if (!root.is_object()) {
        return false;
    }

    std::string format;
    if (!parseString(root, "format", format) || format != kWorldDataFormat) {
        return false;
    }

    const auto& version = root.at("version");
    if (!version.is_number_integer() || version.get<int>() != kWorldDataVersion) {
        return false;
    }

    const auto& world = root.at("world");
    const auto& bounds = world.at("bounds");
    if (!world.is_object() || !bounds.is_object() ||
        !parsePosition(bounds.at("min"), result.worldMin) ||
        !parsePosition(bounds.at("max"), result.worldMax) ||
        result.worldMin.x > result.worldMax.x ||
        result.worldMin.y > result.worldMax.y ||
        result.worldMin.z > result.worldMax.z ||
        !parseFiniteNumber(world, "simulation_speed", result.simulationSpeed) ||
        result.simulationSpeed < 0.0 ||
        !parseTimestamp(world, "last_update_ns", result.lastUpdate)) {
        return false;
    }

    const auto& autoUpdate = world.at("auto_update");
    if (!autoUpdate.is_object() ||
        !parseBoolean(autoUpdate, "enabled", result.autoUpdateEnabled) ||
        !parseFiniteNumber(autoUpdate, "interval", result.updateInterval) ||
        result.updateInterval < 0.0) {
        return false;
    }

    const auto& environments = root.at("environments");
    if (!environments.is_array()) {
        return false;
    }
    std::set<std::string> environmentIds;
    result.environments.reserve(environments.size());
    for (const auto& value : environments) {
        if (!value.is_object()) {
            return false;
        }
        WorldEnvironment environment;
        if (!parseString(value, "id", environment.id) || environment.id.empty() ||
            !environmentIds.insert(environment.id).second ||
            !parseString(value, "name", environment.name) ||
            !parseString(value, "description", environment.description) ||
            !parseString(value, "type", environment.type) ||
            !parsePosition(value.at("center"), environment.center) ||
            !parseFiniteNumber(value, "radius", environment.radius) ||
            environment.radius < 0.0 ||
            !parseStringMap(value.at("properties"), environment.properties) ||
            !parseTimestamp(value, "created_at_ns", environment.createdAt) ||
            !parseBoolean(value, "active", environment.active)) {
            return false;
        }
        result.environments.push_back(std::move(environment));
    }

    const auto& agents = root.at("agents");
    if (!agents.is_array()) {
        return false;
    }
    std::set<std::string> agentIds;
    result.agents.reserve(agents.size());
    for (const auto& value : agents) {
        if (!value.is_object()) {
            return false;
        }
        WorldAgent agent;
        if (!parseString(value, "agent_id", agent.agentId) || agent.agentId.empty() ||
            !agentIds.insert(agent.agentId).second ||
            !parseString(value, "name", agent.name) ||
            !parseString(value, "type", agent.type) ||
            !parsePosition(value.at("position"), agent.position) ||
            !isWithinBounds(agent.position, result.worldMin, result.worldMax) ||
            !parsePosition(value.at("velocity"), agent.velocity) ||
            !parseFiniteNumber(value, "interaction_radius", agent.interactionRadius) ||
            agent.interactionRadius < 0.0 ||
            !parseString(value, "current_environment", agent.currentEnvironment) ||
            (!agent.currentEnvironment.empty() &&
             environmentIds.count(agent.currentEnvironment) == 0) ||
            !parseStringMap(value.at("attributes"), agent.attributes) ||
            !parseTimestamp(value, "last_update_ns", agent.lastUpdate) ||
            !parseBoolean(value, "online", agent.online)) {
            return false;
        }
        result.agents.push_back(std::move(agent));
    }

    const auto& interactions = root.at("interactions");
    if (!interactions.is_array()) {
        return false;
    }
    std::set<std::string> interactionIds;
    result.interactions.reserve(interactions.size());
    for (const auto& value : interactions) {
        if (!value.is_object()) {
            return false;
        }
        WorldInteraction interaction;
        if (!parseString(value, "id", interaction.id) || interaction.id.empty() ||
            !interactionIds.insert(interaction.id).second ||
            !parseString(value, "initiator_id", interaction.initiatorId) ||
            agentIds.count(interaction.initiatorId) == 0 ||
            !parseString(value, "target_id", interaction.targetId) ||
            agentIds.count(interaction.targetId) == 0 ||
            !parseString(value, "type", interaction.type) ||
            !parsePosition(value.at("location"), interaction.location) ||
            !parseTimestamp(value, "timestamp_ns", interaction.timestamp) ||
            !parseStringMap(value.at("metadata"), interaction.metadata) ||
            !parseBoolean(value, "completed", interaction.completed)) {
            return false;
        }
        result.interactions.push_back(std::move(interaction));
    }

    return true;
}

} // namespace

namespace elizaos {

// WorldPosition methods
double WorldPosition::distanceTo(const WorldPosition& other) const {
    double dx = x - other.x;
    double dy = y - other.y;
    double dz = z - other.z;
    return std::sqrt(dx*dx + dy*dy + dz*dz);
}

WorldPosition WorldPosition::interpolate(const WorldPosition& target, double t) const {
    t = std::max(0.0, std::min(1.0, t)); // Clamp t between 0 and 1
    return WorldPosition(
        x + (target.x - x) * t,
        y + (target.y - y) * t,
        z + (target.z - z) * t
    );
}

// WorldEnvironment methods
bool WorldEnvironment::containsPosition(const WorldPosition& pos) const {
    return distanceFromCenter(pos) <= radius;
}

double WorldEnvironment::distanceFromCenter(const WorldPosition& pos) const {
    return center.distanceTo(pos);
}

// WorldAgent methods
bool WorldAgent::canInteractWith(const WorldAgent& other) const {
    if (!online || !other.online) return false;
    double distance = position.distanceTo(other.position);
    return distance <= (interactionRadius + other.interactionRadius);
}

void WorldAgent::moveTo(const WorldPosition& target, double speed) {
    // Calculate direction std::vector
    WorldPosition direction(
        target.x - position.x,
        target.y - position.y,
        target.z - position.z
    );
    
    double distance = position.distanceTo(target);
    if (distance > 0.0) {
        // Normalize direction and scale by speed
        velocity.x = (direction.x / distance) * speed;
        velocity.y = (direction.y / distance) * speed;
        velocity.z = (direction.z / distance) * speed;
    }
}

void WorldAgent::teleportTo(const WorldPosition& target) {
    position = target;
    velocity = WorldPosition(0, 0, 0);
}

// ElizasWorld implementation
ElizasWorld::ElizasWorld() 
    : worldMin_(-1000, -1000, -1000)
    , worldMax_(1000, 1000, 1000)
    , simulationSpeed_(1.0)
    , autoUpdateEnabled_(false)
    , updateInterval_(0.1)
    , lastUpdate_(std::chrono::system_clock::now())
{
}

bool ElizasWorld::addEnvironment(const WorldEnvironment& environment) {
    auto it = findEnvironment(environment.id);
    if (it != environments_.end()) {
        return false; // Environment already exists
    }
    
    environments_.push_back(environment);
    
    if (onEnvironmentChangeCallback_) {
        onEnvironmentChangeCallback_("environment_added", environment.id);
    }
    
    return true;
}

bool ElizasWorld::removeEnvironment(const std::string& environmentId) {
    auto it = findEnvironment(environmentId);
    if (it == environments_.end()) {
        return false;
    }
    
    environments_.erase(it);
    
    // Move agents out of removed environment
    for (auto& agent : agents_) {
        if (agent.currentEnvironment == environmentId) {
            agent.currentEnvironment.clear();
        }
    }
    
    if (onEnvironmentChangeCallback_) {
        onEnvironmentChangeCallback_("environment_removed", environmentId);
    }
    
    return true;
}

std::optional<WorldEnvironment> ElizasWorld::getEnvironment(const std::string& environmentId) const {
    auto it = findEnvironment(environmentId);
    if (it != environments_.end()) {
        return *it;
    }
    return std::nullopt;
}

std::vector<WorldEnvironment> ElizasWorld::getAllEnvironments() const {
    return environments_;
}

std::vector<WorldEnvironment> ElizasWorld::getActiveEnvironments() const {
    std::vector<WorldEnvironment> active;
    std::copy_if(environments_.begin(), environments_.end(), 
                 std::back_inserter(active),
                 [](const WorldEnvironment& env) { return env.active; });
    return active;
}

bool ElizasWorld::updateEnvironment(const WorldEnvironment& environment) {
    auto it = findEnvironment(environment.id);
    if (it == environments_.end()) {
        return false;
    }
    
    *it = environment;
    
    if (onEnvironmentChangeCallback_) {
        onEnvironmentChangeCallback_("environment_updated", environment.id);
    }
    
    return true;
}

bool ElizasWorld::addAgent(const WorldAgent& agent) {
    auto it = findAgent(agent.agentId);
    if (it != agents_.end()) {
        return false; // Agent already exists
    }
    
    if (!isWithinWorldBounds(agent.position)) {
        return false; // Position outside world bounds
    }
    
    agents_.push_back(agent);
    
    // Update agent's current environment
    std::string envId = findEnvironmentForPosition(agent.position);
    if (!envId.empty()) {
        agents_.back().currentEnvironment = envId;
        if (onAgentEnterEnvCallback_) {
            onAgentEnterEnvCallback_(agent.agentId, envId);
        }
    }
    
    return true;
}

bool ElizasWorld::removeAgent(const std::string& agentId) {
    auto it = findAgent(agentId);
    if (it == agents_.end()) {
        return false;
    }
    
    std::string currentEnv = it->currentEnvironment;
    agents_.erase(it);
    
    if (!currentEnv.empty() && onAgentLeaveEnvCallback_) {
        onAgentLeaveEnvCallback_(agentId, currentEnv);
    }
    
    return true;
}

std::optional<WorldAgent> ElizasWorld::getAgent(const std::string& agentId) const {
    auto it = findAgent(agentId);
    if (it != agents_.end()) {
        return *it;
    }
    return std::nullopt;
}

std::vector<WorldAgent> ElizasWorld::getAllAgents() const {
    return agents_;
}

std::vector<WorldAgent> ElizasWorld::getOnlineAgents() const {
    std::vector<WorldAgent> online;
    std::copy_if(agents_.begin(), agents_.end(), 
                 std::back_inserter(online),
                 [](const WorldAgent& agent) { return agent.online; });
    return online;
}

std::vector<WorldAgent> ElizasWorld::getAgentsInEnvironment(const std::string& environmentId) const {
    std::vector<WorldAgent> inEnv;
    std::copy_if(agents_.begin(), agents_.end(), 
                 std::back_inserter(inEnv),
                 [&environmentId](const WorldAgent& agent) { 
                     return agent.currentEnvironment == environmentId; 
                 });
    return inEnv;
}

bool ElizasWorld::updateAgent(const WorldAgent& agent) {
    auto it = findAgent(agent.agentId);
    if (it == agents_.end()) {
        return false;
    }
    
    std::string oldEnv = it->currentEnvironment;
    *it = agent;
    
    // Check for environment changes
    std::string newEnv = findEnvironmentForPosition(agent.position);
    if (newEnv != oldEnv) {
        triggerAgentEnvironmentChange(agent.agentId, oldEnv, newEnv);
        it->currentEnvironment = newEnv;
    }
    
    return true;
}

std::vector<WorldAgent> ElizasWorld::getAgentsNearPosition(const WorldPosition& pos, double radius) const {
    std::vector<WorldAgent> nearby;
    for (const auto& agent : agents_) {
        if (agent.online && agent.position.distanceTo(pos) <= radius) {
            nearby.push_back(agent);
        }
    }
    return nearby;
}

std::vector<WorldAgent> ElizasWorld::getAgentsInRadius(const std::string& agentId, double radius) const {
    auto it = findAgent(agentId);
    if (it == agents_.end()) {
        return {};
    }
    
    return getAgentsNearPosition(it->position, radius);
}

std::vector<WorldEnvironment> ElizasWorld::getEnvironmentsNearPosition(const WorldPosition& pos) const {
    std::vector<WorldEnvironment> nearby;
    for (const auto& env : environments_) {
        if (env.active && env.containsPosition(pos)) {
            nearby.push_back(env);
        }
    }
    return nearby;
}

std::string ElizasWorld::findEnvironmentForPosition(const WorldPosition& pos) const {
    for (const auto& env : environments_) {
        if (env.active && env.containsPosition(pos)) {
            return env.id;
        }
    }
    return "";
}

bool ElizasWorld::moveAgent(const std::string& agentId, const WorldPosition& target, double speed) {
    auto it = findAgent(agentId);
    if (it == agents_.end()) {
        return false;
    }
    
    if (!isWithinWorldBounds(target)) {
        return false;
    }
    
    it->moveTo(target, speed);
    it->lastUpdate = std::chrono::system_clock::now();
    
    return true;
}

bool ElizasWorld::teleportAgent(const std::string& agentId, const WorldPosition& target) {
    auto it = findAgent(agentId);
    if (it == agents_.end()) {
        return false;
    }
    
    if (!isWithinWorldBounds(target)) {
        return false;
    }
    
    std::string oldEnv = it->currentEnvironment;
    it->teleportTo(target);
    it->lastUpdate = std::chrono::system_clock::now();
    
    // Check for environment change
    std::string newEnv = findEnvironmentForPosition(target);
    if (newEnv != oldEnv) {
        triggerAgentEnvironmentChange(agentId, oldEnv, newEnv);
        it->currentEnvironment = newEnv;
    }
    
    return true;
}

bool ElizasWorld::setAgentVelocity(const std::string& agentId, const WorldPosition& velocity) {
    auto it = findAgent(agentId);
    if (it == agents_.end()) {
        return false;
    }
    
    it->velocity = velocity;
    it->lastUpdate = std::chrono::system_clock::now();
    
    return true;
}

bool ElizasWorld::recordInteraction(const WorldInteraction& interaction) {
    interactions_.push_back(interaction);
    
    if (onInteractionCallback_) {
        onInteractionCallback_(interaction.initiatorId, interaction.targetId);
    }
    
    return true;
}

std::vector<WorldInteraction> ElizasWorld::getInteractionHistory(const std::string& agentId) const {
    std::vector<WorldInteraction> history;
    std::copy_if(interactions_.begin(), interactions_.end(), 
                 std::back_inserter(history),
                 [&agentId](const WorldInteraction& interaction) {
                     return interaction.initiatorId == agentId || interaction.targetId == agentId;
                 });
    return history;
}

std::vector<WorldInteraction> ElizasWorld::getInteractionsInArea(const WorldPosition& center, double radius) const {
    std::vector<WorldInteraction> inArea;
    std::copy_if(interactions_.begin(), interactions_.end(), 
                 std::back_inserter(inArea),
                 [&center, radius](const WorldInteraction& interaction) {
                     return interaction.location.distanceTo(center) <= radius;
                 });
    return inArea;
}

std::vector<WorldInteraction> ElizasWorld::getRecentInteractions(int limit) const {
    std::vector<WorldInteraction> recent = interactions_;
    
    // Sort by timestamp (newest first)
    std::sort(recent.begin(), recent.end(), 
              [](const WorldInteraction& a, const WorldInteraction& b) {
                  return a.timestamp > b.timestamp;
              });
    
    if (recent.size() > static_cast<size_t>(limit)) {
        recent.resize(limit);
    }
    
    return recent;
}

void ElizasWorld::update(double deltaTime) {
    lastUpdate_ = std::chrono::system_clock::now();
    
    processAgentMovements(deltaTime * simulationSpeed_);
    checkEnvironmentTransitions();
    detectProximityInteractions();
    updateEnvironmentStates();
}

void ElizasWorld::processAgentMovements(double deltaTime) {
    for (auto& agent : agents_) {
        if (!agent.online) continue;
        
        // Update position based on velocity
        agent.position.x += agent.velocity.x * deltaTime;
        agent.position.y += agent.velocity.y * deltaTime;
        agent.position.z += agent.velocity.z * deltaTime;
        
        // Clamp to world bounds
        agent.position.x = std::max(worldMin_.x, std::min(worldMax_.x, agent.position.x));
        agent.position.y = std::max(worldMin_.y, std::min(worldMax_.y, agent.position.y));
        agent.position.z = std::max(worldMin_.z, std::min(worldMax_.z, agent.position.z));
        
        agent.lastUpdate = std::chrono::system_clock::now();
    }
}

void ElizasWorld::detectProximityInteractions() {
    auto now = std::chrono::system_clock::now();
    
    for (size_t i = 0; i < agents_.size(); ++i) {
        for (size_t j = i + 1; j < agents_.size(); ++j) {
            if (agents_[i].canInteractWith(agents_[j])) {
                // Create a proximity interaction
                WorldInteraction interaction;
                interaction.id = generateInteractionId();
                interaction.initiatorId = agents_[i].agentId;
                interaction.targetId = agents_[j].agentId;
                interaction.type = "proximity";
                interaction.location = agents_[i].position;
                interaction.timestamp = now;
                interaction.metadata["distance"] = std::to_string(
                    agents_[i].position.distanceTo(agents_[j].position));
                
                recordInteraction(interaction);
            }
        }
    }
}

void ElizasWorld::updateEnvironmentStates() {
    // Update environment activity based on agent presence
    for (auto& env : environments_) {
        auto agentsInEnv = getAgentsInEnvironment(env.id);
        env.properties["agent_count"] = std::to_string(agentsInEnv.size());
        env.properties["last_updated"] = std::to_string(
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());
    }
}

void ElizasWorld::onAgentEnterEnvironment(WorldEventCallback callback) {
    onAgentEnterEnvCallback_ = callback;
}

void ElizasWorld::onAgentLeaveEnvironment(WorldEventCallback callback) {
    onAgentLeaveEnvCallback_ = callback;
}

void ElizasWorld::onAgentInteraction(WorldEventCallback callback) {
    onInteractionCallback_ = callback;
}

void ElizasWorld::onEnvironmentChange(WorldEventCallback callback) {
    onEnvironmentChangeCallback_ = callback;
}

size_t ElizasWorld::getAgentCount() const {
    return agents_.size();
}

size_t ElizasWorld::getEnvironmentCount() const {
    return environments_.size();
}

size_t ElizasWorld::getInteractionCount() const {
    return interactions_.size();
}

double ElizasWorld::getWorldActivity() const {
    auto now = std::chrono::system_clock::now();
    auto oneHourAgo = now - std::chrono::hours(1);
    
    size_t recentInteractions = 0;
    for (const auto& interaction : interactions_) {
        if (interaction.timestamp > oneHourAgo) {
            recentInteractions++;
        }
    }
    
    return static_cast<double>(recentInteractions);
}

std::map<std::string, int> ElizasWorld::getEnvironmentPopulations() const {
    std::map<std::string, int> populations;
    for (const auto& env : environments_) {
        populations[env.id] = getAgentsInEnvironment(env.id).size();
    }
    return populations;
}

std::vector<std::string> ElizasWorld::getMostActiveAgents(int limit) const {
    std::map<std::string, int> activityCounts;
    
    for (const auto& interaction : interactions_) {
        activityCounts[interaction.initiatorId]++;
        activityCounts[interaction.targetId]++;
    }
    
    std::vector<std::pair<std::string, int>> sortedActivity(
        activityCounts.begin(), activityCounts.end());
    
    std::sort(sortedActivity.begin(), sortedActivity.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<std::string> result;
    for (size_t i = 0; i < std::min(static_cast<size_t>(limit), sortedActivity.size()); ++i) {
        result.push_back(sortedActivity[i].first);
    }
    
    return result;
}

bool ElizasWorld::saveWorldState(const std::string& filePath) const {
    namespace fs = std::filesystem;
    if (filePath.empty()) {
        return false;
    }

    try {
        const fs::path destination(filePath);
        fs::path parent = destination.parent_path();
        if (parent.empty()) {
            parent = fs::current_path();
        }

        std::error_code error;
        if (!fs::is_directory(parent, error) || error) {
            return false;
        }

        static std::atomic<std::uint64_t> temporaryFileCounter{0};
        fs::path temporary;
        for (int attempt = 0; attempt < 100; ++attempt) {
            temporary = destination;
            temporary += ".tmp." + std::to_string(
                temporaryFileCounter.fetch_add(1, std::memory_order_relaxed));
            error.clear();
            if (!fs::exists(temporary, error) && !error) {
                break;
            }
            temporary.clear();
        }
        if (temporary.empty()) {
            return false;
        }

        const std::string data = exportWorldData();
        std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
        if (!output.is_open()) {
            return false;
        }
        output.write(data.data(), static_cast<std::streamsize>(data.size()));
        output.flush();
        if (!output) {
            output.close();
            fs::remove(temporary, error);
            return false;
        }
        output.close();
        if (!output) {
            fs::remove(temporary, error);
            return false;
        }

        error.clear();
        fs::rename(temporary, destination, error);
        if (error) {
            std::error_code cleanupError;
            fs::remove(temporary, cleanupError);
            return false;
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool ElizasWorld::loadWorldState(const std::string& filePath) {
    if (filePath.empty()) {
        return false;
    }

    try {
        std::error_code error;
        if (!std::filesystem::is_regular_file(filePath, error) || error) {
            return false;
        }
        std::ifstream input(filePath, std::ios::binary);
        if (!input.is_open()) {
            return false;
        }
        const std::string data((std::istreambuf_iterator<char>(input)),
                               std::istreambuf_iterator<char>());
        if (input.bad()) {
            return false;
        }
        return importWorldData(data);
    } catch (...) {
        return false;
    }
}

std::string ElizasWorld::exportWorldData() const {
    json environments = json::array();
    for (const auto& environment : environments_) {
        environments.push_back(environmentToJson(environment));
    }

    json agents = json::array();
    for (const auto& agent : agents_) {
        agents.push_back(agentToJson(agent));
    }

    json interactions = json::array();
    for (const auto& interaction : interactions_) {
        interactions.push_back(interactionToJson(interaction));
    }

    const json root = {
        {"agents", std::move(agents)},
        {"environments", std::move(environments)},
        {"format", kWorldDataFormat},
        {"interactions", std::move(interactions)},
        {"version", kWorldDataVersion},
        {"world", {
            {"auto_update", {
                {"enabled", autoUpdateEnabled_},
                {"interval", updateInterval_}
            }},
            {"bounds", {
                {"max", positionToJson(worldMax_)},
                {"min", positionToJson(worldMin_)}
            }},
            {"last_update_ns", timePointToNanoseconds(lastUpdate_)},
            {"simulation_speed", simulationSpeed_}
        }}
    };
    return root.dump(2);
}

bool ElizasWorld::importWorldData(const std::string& data) {
    try {
        const json root = json::parse(data, nullptr, false);
        if (root.is_discarded()) {
            return false;
        }

        ParsedWorldState parsed;
        if (!parseWorldDocument(root, parsed)) {
            return false;
        }

        environments_.swap(parsed.environments);
        agents_.swap(parsed.agents);
        interactions_.swap(parsed.interactions);
        worldMin_ = parsed.worldMin;
        worldMax_ = parsed.worldMax;
        simulationSpeed_ = parsed.simulationSpeed;
        autoUpdateEnabled_ = parsed.autoUpdateEnabled;
        updateInterval_ = parsed.updateInterval;
        lastUpdate_ = parsed.lastUpdate;
        return true;
    } catch (...) {
        return false;
    }
}

void ElizasWorld::setWorldBounds(const WorldPosition& min, const WorldPosition& max) {
    worldMin_ = min;
    worldMax_ = max;
}

void ElizasWorld::setSimulationSpeed(double speed) {
    simulationSpeed_ = std::max(0.0, speed);
}

void ElizasWorld::setAutoUpdate(bool enabled, double interval) {
    autoUpdateEnabled_ = enabled;
    updateInterval_ = interval;
}

void ElizasWorld::checkEnvironmentTransitions() {
    for (auto& agent : agents_) {
        if (!agent.online) continue;
        
        std::string currentEnv = agent.currentEnvironment;
        std::string newEnv = findEnvironmentForPosition(agent.position);
        
        if (newEnv != currentEnv) {
            triggerAgentEnvironmentChange(agent.agentId, currentEnv, newEnv);
            agent.currentEnvironment = newEnv;
        }
    }
}

bool ElizasWorld::isWithinWorldBounds(const WorldPosition& pos) const {
    return pos.x >= worldMin_.x && pos.x <= worldMax_.x &&
           pos.y >= worldMin_.y && pos.y <= worldMax_.y &&
           pos.z >= worldMin_.z && pos.z <= worldMax_.z;
}

std::string ElizasWorld::generateInteractionId() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    ss << "int_";
    for (int i = 0; i < 8; ++i) {
        ss << std::hex << dis(gen);
    }
    return ss.str();
}

void ElizasWorld::triggerAgentEnvironmentChange(const std::string& agentId, 
                                              const std::string& oldEnv, 
                                              const std::string& newEnv) {
    if (!oldEnv.empty() && onAgentLeaveEnvCallback_) {
        onAgentLeaveEnvCallback_(agentId, oldEnv);
    }
    
    if (!newEnv.empty() && onAgentEnterEnvCallback_) {
        onAgentEnterEnvCallback_(agentId, newEnv);
    }
}

std::vector<WorldAgent>::iterator ElizasWorld::findAgent(const std::string& agentId) {
    return std::find_if(agents_.begin(), agents_.end(),
                        [&agentId](const WorldAgent& agent) { return agent.agentId == agentId; });
}

std::vector<WorldAgent>::const_iterator ElizasWorld::findAgent(const std::string& agentId) const {
    return std::find_if(agents_.begin(), agents_.end(),
                        [&agentId](const WorldAgent& agent) { return agent.agentId == agentId; });
}

std::vector<WorldEnvironment>::iterator ElizasWorld::findEnvironment(const std::string& environmentId) {
    return std::find_if(environments_.begin(), environments_.end(),
                        [&environmentId](const WorldEnvironment& env) { return env.id == environmentId; });
}

std::vector<WorldEnvironment>::const_iterator ElizasWorld::findEnvironment(const std::string& environmentId) const {
    return std::find_if(environments_.begin(), environments_.end(),
                        [&environmentId](const WorldEnvironment& env) { return env.id == environmentId; });
}

} // namespace elizaos
