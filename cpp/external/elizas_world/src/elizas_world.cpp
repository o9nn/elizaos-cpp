#include "elizaos/elizas_world.hpp"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <random>
#include <filesystem>
#include <fstream>
#include <set>
#include <system_error>

#include <nlohmann/json.hpp>

namespace elizaos {
namespace {

namespace fs = std::filesystem;
using json = nlohmann::json;

constexpr int kWorldStateVersion = 1;

std::int64_t toMilliseconds(std::chrono::system_clock::time_point value) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               value.time_since_epoch()).count();
}

std::chrono::system_clock::time_point fromMilliseconds(std::int64_t value) {
    return std::chrono::system_clock::time_point(std::chrono::milliseconds(value));
}

json positionToJson(const WorldPosition& value) {
    return {{"x", value.x}, {"y", value.y}, {"z", value.z}};
}

bool finitePosition(const WorldPosition& value) {
    return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
}

WorldPosition positionFromJson(const json& value) {
    if (!value.is_object()) throw std::invalid_argument("position must be an object");
    WorldPosition position(value.at("x").get<double>(), value.at("y").get<double>(),
                           value.at("z").get<double>());
    if (!finitePosition(position)) throw std::invalid_argument("position must be finite");
    return position;
}

json environmentToJson(const WorldEnvironment& environment) {
    return {
        {"id", environment.id}, {"name", environment.name},
        {"description", environment.description}, {"type", environment.type},
        {"center", positionToJson(environment.center)}, {"radius", environment.radius},
        {"properties", environment.properties},
        {"created_at_ms", toMilliseconds(environment.createdAt)},
        {"active", environment.active}
    };
}

WorldEnvironment environmentFromJson(const json& value) {
    WorldEnvironment environment;
    environment.id = value.at("id").get<std::string>();
    environment.name = value.at("name").get<std::string>();
    environment.description = value.at("description").get<std::string>();
    environment.type = value.at("type").get<std::string>();
    environment.center = positionFromJson(value.at("center"));
    environment.radius = value.at("radius").get<double>();
    environment.properties = value.at("properties").get<std::map<std::string, std::string>>();
    environment.createdAt = fromMilliseconds(value.at("created_at_ms").get<std::int64_t>());
    environment.active = value.at("active").get<bool>();
    if (environment.id.empty() || !std::isfinite(environment.radius) || environment.radius < 0.0) {
        throw std::invalid_argument("environment id/radius is invalid");
    }
    return environment;
}

json agentToJson(const WorldAgent& agent) {
    return {
        {"agent_id", agent.agentId}, {"name", agent.name}, {"type", agent.type},
        {"position", positionToJson(agent.position)},
        {"velocity", positionToJson(agent.velocity)},
        {"interaction_radius", agent.interactionRadius},
        {"current_environment", agent.currentEnvironment},
        {"attributes", agent.attributes},
        {"last_update_ms", toMilliseconds(agent.lastUpdate)}, {"online", agent.online}
    };
}

WorldAgent agentFromJson(const json& value) {
    WorldAgent agent;
    agent.agentId = value.at("agent_id").get<std::string>();
    agent.name = value.at("name").get<std::string>();
    agent.type = value.at("type").get<std::string>();
    agent.position = positionFromJson(value.at("position"));
    agent.velocity = positionFromJson(value.at("velocity"));
    agent.interactionRadius = value.at("interaction_radius").get<double>();
    agent.currentEnvironment = value.at("current_environment").get<std::string>();
    agent.attributes = value.at("attributes").get<std::map<std::string, std::string>>();
    agent.lastUpdate = fromMilliseconds(value.at("last_update_ms").get<std::int64_t>());
    agent.online = value.at("online").get<bool>();
    if (agent.agentId.empty() || !std::isfinite(agent.interactionRadius) ||
        agent.interactionRadius < 0.0) {
        throw std::invalid_argument("agent id/interaction radius is invalid");
    }
    return agent;
}

json interactionToJson(const WorldInteraction& interaction) {
    return {
        {"id", interaction.id}, {"initiator_id", interaction.initiatorId},
        {"target_id", interaction.targetId}, {"type", interaction.type},
        {"location", positionToJson(interaction.location)},
        {"timestamp_ms", toMilliseconds(interaction.timestamp)},
        {"metadata", interaction.metadata}, {"completed", interaction.completed}
    };
}

WorldInteraction interactionFromJson(const json& value) {
    WorldInteraction interaction;
    interaction.id = value.at("id").get<std::string>();
    interaction.initiatorId = value.at("initiator_id").get<std::string>();
    interaction.targetId = value.at("target_id").get<std::string>();
    interaction.type = value.at("type").get<std::string>();
    interaction.location = positionFromJson(value.at("location"));
    interaction.timestamp = fromMilliseconds(value.at("timestamp_ms").get<std::int64_t>());
    interaction.metadata = value.at("metadata").get<std::map<std::string, std::string>>();
    interaction.completed = value.at("completed").get<bool>();
    if (interaction.id.empty()) throw std::invalid_argument("interaction id is empty");
    return interaction;
}

}  // namespace

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
    if (filePath.empty()) return false;
    try {
        const fs::path destination(filePath);
        std::error_code ec;
        if (destination.has_parent_path()) {
            fs::create_directories(destination.parent_path(), ec);
            if (ec) return false;
        }
        fs::path temporary = destination;
        temporary += ".tmp";
        {
            std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
            if (!output) return false;
            output << exportWorldData() << '\n';
            if (!output.good()) {
                output.close();
                fs::remove(temporary, ec);
                return false;
            }
        }
        fs::rename(temporary, destination, ec);
        if (ec) {
            // Windows does not replace an existing destination atomically.
            fs::remove(destination, ec);
            ec.clear();
            fs::rename(temporary, destination, ec);
        }
        if (ec) {
            fs::remove(temporary, ec);
            return false;
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool ElizasWorld::loadWorldState(const std::string& filePath) {
    if (filePath.empty()) return false;
    std::ifstream input(filePath, std::ios::binary);
    if (!input) return false;
    std::ostringstream buffer;
    buffer << input.rdbuf();
    if ((!input.good() && !input.eof()) || buffer.str().empty()) return false;
    return importWorldData(buffer.str());
}

std::string ElizasWorld::exportWorldData() const {
    json environments = json::array();
    for (const auto& environment : environments_) environments.push_back(environmentToJson(environment));
    json agents = json::array();
    for (const auto& agent : agents_) agents.push_back(agentToJson(agent));
    json interactions = json::array();
    for (const auto& interaction : interactions_) interactions.push_back(interactionToJson(interaction));

    const json document = {
        {"schema", "elizaos.world_state"},
        {"version", kWorldStateVersion},
        {"world_bounds", {{"min", positionToJson(worldMin_)}, {"max", positionToJson(worldMax_)}}},
        {"simulation", {
            {"speed", simulationSpeed_}, {"auto_update", autoUpdateEnabled_},
            {"update_interval", updateInterval_},
            {"last_update_ms", toMilliseconds(lastUpdate_)}
        }},
        {"environments", std::move(environments)},
        {"agents", std::move(agents)},
        {"interactions", std::move(interactions)}
    };
    return document.dump(2);
}

bool ElizasWorld::importWorldData(const std::string& data) {
    try {
        const json document = json::parse(data);
        if (!document.is_object() || document.value("schema", "") != "elizaos.world_state" ||
            document.value("version", 0) != kWorldStateVersion) {
            return false;
        }

        const auto& bounds = document.at("world_bounds");
        WorldPosition newMin = positionFromJson(bounds.at("min"));
        WorldPosition newMax = positionFromJson(bounds.at("max"));
        if (newMin.x > newMax.x || newMin.y > newMax.y || newMin.z > newMax.z) return false;

        const auto& simulation = document.at("simulation");
        const double newSpeed = simulation.at("speed").get<double>();
        const double newInterval = simulation.at("update_interval").get<double>();
        if (!std::isfinite(newSpeed) || newSpeed < 0.0 || !std::isfinite(newInterval) ||
            newInterval <= 0.0) return false;
        const bool newAutoUpdate = simulation.at("auto_update").get<bool>();
        const auto newLastUpdate = fromMilliseconds(
            simulation.at("last_update_ms").get<std::int64_t>());

        const auto& environmentValues = document.at("environments");
        const auto& agentValues = document.at("agents");
        const auto& interactionValues = document.at("interactions");
        if (!environmentValues.is_array() || !agentValues.is_array() ||
            !interactionValues.is_array()) return false;

        std::vector<WorldEnvironment> newEnvironments;
        std::vector<WorldAgent> newAgents;
        std::vector<WorldInteraction> newInteractions;
        std::set<std::string> environmentIds;
        std::set<std::string> agentIds;
        std::set<std::string> interactionIds;

        for (const auto& value : environmentValues) {
            auto environment = environmentFromJson(value);
            if (!environmentIds.insert(environment.id).second) return false;
            newEnvironments.push_back(std::move(environment));
        }
        for (const auto& value : agentValues) {
            auto agent = agentFromJson(value);
            if (!agentIds.insert(agent.agentId).second) return false;
            if (!agent.currentEnvironment.empty() &&
                environmentIds.count(agent.currentEnvironment) == 0) return false;
            if (agent.position.x < newMin.x || agent.position.x > newMax.x ||
                agent.position.y < newMin.y || agent.position.y > newMax.y ||
                agent.position.z < newMin.z || agent.position.z > newMax.z) return false;
            newAgents.push_back(std::move(agent));
        }
        for (const auto& value : interactionValues) {
            auto interaction = interactionFromJson(value);
            if (!interactionIds.insert(interaction.id).second) return false;
            if ((!interaction.initiatorId.empty() && agentIds.count(interaction.initiatorId) == 0) ||
                (!interaction.targetId.empty() && agentIds.count(interaction.targetId) == 0)) return false;
            newInteractions.push_back(std::move(interaction));
        }

        // Commit only after the complete document has parsed and validated.
        environments_.swap(newEnvironments);
        agents_.swap(newAgents);
        interactions_.swap(newInteractions);
        worldMin_ = newMin;
        worldMax_ = newMax;
        simulationSpeed_ = newSpeed;
        autoUpdateEnabled_ = newAutoUpdate;
        updateInterval_ = newInterval;
        lastUpdate_ = newLastUpdate;
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
