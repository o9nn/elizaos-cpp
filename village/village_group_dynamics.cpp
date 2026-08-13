#include "village_group_dynamics.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <sstream>

namespace elizaos {
namespace village {

using json = nlohmann::json;

// ============================================================================
// SocialNetwork Implementation
// ============================================================================

void SocialNetwork::addResident(const ResidentProfile& profile) {
    std::unique_lock<std::mutex> lock(mutex_);
    profiles_[profile.id] = profile;
}

void SocialNetwork::removeResident(const ResidentId& id) {
    std::unique_lock<std::mutex> lock(mutex_);
    profiles_.erase(id);
    // Remove all edges involving this resident
    for (auto it = edges_.begin(); it != edges_.end();) {
        if (it->first.first == id || it->first.second == id)
            it = edges_.erase(it);
        else ++it;
    }
}

void SocialNetwork::updateRelationship(const ResidentId& a, const ResidentId& b,
                                        const std::string& interactionType, double valence) {
    std::unique_lock<std::mutex> lock(mutex_);
    auto key = std::make_pair(std::min(a, b), std::max(a, b));
    auto& rel = edges_[key];
    rel.interactionCount++;
    rel.lastInteraction = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();

    // Update trust based on interaction valence
    rel.trust = std::clamp(rel.trust + valence * 0.1, -1.0, 1.0);
    // Familiarity grows with interaction
    rel.familiarity = std::min(1.0, rel.familiarity + 0.05);
    // Influence grows for positive interactions
    if (valence > 0) rel.influence = std::min(1.0, rel.influence + 0.02);

    // Specific interaction type effects
    if (interactionType == "collaboration") {
        rel.trust += 0.05; rel.affinity += 0.03;
    } else if (interactionType == "conflict") {
        rel.trust -= 0.1; rel.affinity -= 0.05;
    } else if (interactionType == "conversation") {
        rel.familiarity += 0.02;
    } else if (interactionType == "help") {
        rel.trust += 0.08; rel.influence += 0.03;
    }
    rel.trust = std::clamp(rel.trust, -1.0, 1.0);
    rel.affinity = std::clamp(rel.affinity, -1.0, 1.0);
}

Relationship SocialNetwork::getRelationship(const ResidentId& a, const ResidentId& b) const {
    std::unique_lock<std::mutex> lock(mutex_);
    auto key = std::make_pair(std::min(a, b), std::max(a, b));
    auto it = edges_.find(key);
    return (it != edges_.end()) ? it->second : Relationship{};
}

double SocialNetwork::getInfluenceWeight(const ResidentId& source, const ResidentId& target) const {
    auto rel = getRelationship(source, target);
    return std::max(0.0, rel.trust * 0.5 + rel.influence * 0.3 + rel.familiarity * 0.2);
}

std::vector<ResidentId> SocialNetwork::getNeighbors(const ResidentId& id) const {
    std::unique_lock<std::mutex> lock(mutex_);
    std::vector<ResidentId> neighbors;
    for (const auto& [key, rel] : edges_) {
        if (key.first == id) neighbors.push_back(key.second);
        else if (key.second == id) neighbors.push_back(key.first);
    }
    return neighbors;
}

double SocialNetwork::calculateCentrality(const ResidentId& id) const {
    auto neighbors = getNeighbors(id);
    if (profiles_.empty()) return 0.0;
    return static_cast<double>(neighbors.size()) / (profiles_.size() - 1);
}

std::vector<std::set<ResidentId>> SocialNetwork::detectCommunities() const {
    std::unique_lock<std::mutex> lock(mutex_);
    // Simple connected-components with affinity threshold
    std::set<ResidentId> visited;
    std::vector<std::set<ResidentId>> communities;
    for (const auto& [id, _] : profiles_) {
        if (visited.count(id)) continue;
        std::set<ResidentId> community;
        std::vector<ResidentId> queue = {id};
        while (!queue.empty()) {
            auto current = queue.back(); queue.pop_back();
            if (visited.count(current)) continue;
            visited.insert(current);
            community.insert(current);
            for (const auto& [key, rel] : edges_) {
                if (rel.compositeScore() < 0.2) continue;
                ResidentId neighbor;
                if (key.first == current) neighbor = key.second;
                else if (key.second == current) neighbor = key.first;
                else continue;
                if (!visited.count(neighbor)) queue.push_back(neighbor);
            }
        }
        if (community.size() >= 2) communities.push_back(community);
    }
    return communities;
}

std::set<ResidentId> SocialNetwork::propagateInformation(const ResidentId& source,
                                                          const std::string& /*info*/,
                                                          int maxHops) const {
    std::set<ResidentId> reached = {source};
    std::vector<ResidentId> frontier = getNeighbors(source);
    static std::mt19937 rng(42);
    for (int hop = 0; hop < maxHops && !frontier.empty(); ++hop) {
        std::vector<ResidentId> nextFrontier;
        for (const auto& agent : frontier) {
            if (reached.count(agent)) continue;
            double weight = getInfluenceWeight(source, agent);
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            if (dist(rng) < weight) {
                reached.insert(agent);
                auto neighbors = getNeighbors(agent);
                for (const auto& n : neighbors)
                    if (!reached.count(n)) nextFrontier.push_back(n);
            }
        }
        frontier = nextFrontier;
    }
    return reached;
}

const ResidentProfile* SocialNetwork::getProfile(const ResidentId& id) const {
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = profiles_.find(id);
    return (it != profiles_.end()) ? &it->second : nullptr;
}

std::vector<ResidentId> SocialNetwork::getAllResidents() const {
    std::unique_lock<std::mutex> lock(mutex_);
    std::vector<ResidentId> ids;
    for (const auto& [id, _] : profiles_) ids.push_back(id);
    return ids;
}

// ============================================================================
// GroupManager Implementation
// ============================================================================

GroupManager::GroupManager(const GroupDynamicsConfig& config) : config_(config) {}

GroupId GroupManager::generateGroupId() {
    return "group_" + std::to_string(nextGroupId_++);
}

std::vector<GroupManager::ProtoGroup> GroupManager::detectPotentialGroups(
    const SocialNetwork& network) const {
    std::vector<ProtoGroup> protos;
    auto communities = network.detectCommunities();
    for (const auto& community : communities) {
        // Check if this community already forms an existing group
        bool alreadyGrouped = false;
        for (const auto& [_, group] : groups_) {
            if (group.members == community) { alreadyGrouped = true; break; }
        }
        if (alreadyGrouped) continue;

        // Calculate mean affinity
        double totalAffinity = 0.0; int pairs = 0;
        for (auto it = community.begin(); it != community.end(); ++it) {
            for (auto jt = std::next(it); jt != community.end(); ++jt) {
                totalAffinity += network.getRelationship(*it, *jt).compositeScore();
                pairs++;
            }
        }
        double meanAffinity = pairs > 0 ? totalAffinity / pairs : 0.0;
        if (meanAffinity >= config_.formationAffinityThreshold) {
            ProtoGroup pg;
            pg.candidates = community;
            pg.affinityScore = meanAffinity;
            pg.trigger = "affinity_threshold";
            protos.push_back(pg);
        }
    }
    return protos;
}

GroupId GroupManager::formGroup(const ProtoGroup& proto, const std::string& triggerEvent) {
    std::unique_lock<std::mutex> lock(mutex_);
    GroupId id = generateGroupId();
    Group group;
    group.id = id;
    group.members = proto.candidates;
    group.formationTime = currentTick_;
    group.cohesion = std::clamp(proto.affinityScore, 0.0, 1.0);
    groups_[id] = group;
    lastCohesionUpdateTick_[id] = currentTick_;
    emitEvent("group.formed", id, "{\"trigger\":\"" + triggerEvent +
              "\",\"members\":" + std::to_string(proto.candidates.size()) + "}");
    return id;
}

void GroupManager::dissolveGroup(const GroupId& id, const std::string& reason) {
    std::unique_lock<std::mutex> lock(mutex_);
    emitEvent("group.dissolved", id, "{\"reason\":\"" + reason + "\"}");
    groups_.erase(id);
    conversations_.erase(id);
    lastCohesionUpdateTick_.erase(id);
}

GroupId GroupManager::mergeGroups(const GroupId& a, const GroupId& b) {
    std::unique_lock<std::mutex> lock(mutex_);
    auto itA = groups_.find(a), itB = groups_.find(b);
    if (itA == groups_.end() || itB == groups_.end()) return "";
    ProtoGroup merged;
    merged.candidates = itA->second.members;
    merged.candidates.insert(itB->second.members.begin(), itB->second.members.end());
    merged.affinityScore = (itA->second.cohesion + itB->second.cohesion) / 2.0;
    groups_.erase(a);
    groups_.erase(b);
    conversations_.erase(a);
    conversations_.erase(b);
    lastCohesionUpdateTick_.erase(a);
    lastCohesionUpdateTick_.erase(b);
    lock.unlock();
    return formGroup(merged, "merge");
}

std::pair<GroupId, GroupId> GroupManager::splitGroup(const GroupId& id,
                                                     const std::set<ResidentId>& factionA,
                                                     const std::set<ResidentId>& factionB) {
    dissolveGroup(id, "split");
    ProtoGroup pgA, pgB;
    pgA.candidates = factionA; pgA.affinityScore = 0.5; pgA.trigger = "split";
    pgB.candidates = factionB; pgB.affinityScore = 0.5; pgB.trigger = "split";
    return {formGroup(pgA, "split"), formGroup(pgB, "split")};
}

void GroupManager::updateCohesion(const GroupId& id, Timestamp currentTick) {
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = groups_.find(id);
    if (it == groups_.end()) return;
    auto& group = it->second;
    currentTick_ = std::max(currentTick_, currentTick);

    Timestamp lastInteractionTick = group.formationTime;
    if (!group.collectiveMemory.empty()) {
        lastInteractionTick =
            std::max(lastInteractionTick, group.collectiveMemory.back().timestamp);
    }

    auto cursorIt =
        lastCohesionUpdateTick_.try_emplace(id, group.formationTime).first;
    const Timestamp baselineTick =
        std::max(cursorIt->second, lastInteractionTick);
    if (currentTick <= baselineTick) {
        group.cohesion = std::clamp(group.cohesion, 0.0, 1.0);
        return;
    }

    const double elapsedTicks =
        static_cast<double>(currentTick - baselineTick);
    cursorIt->second = currentTick;
    group.cohesion = std::clamp(
        group.cohesion - config_.cohesionDecayRate * (elapsedTicks / 60.0),
        0.0,
        1.0);
    if (group.cohesion < config_.dissolutionThreshold) {
        lock.unlock();
        dissolveGroup(id, "cohesion_decay");
    }
}

void GroupManager::startGroupConversation(const GroupId& id, const std::string& topic) {
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = groups_.find(id);
    if (it == groups_.end()) return;
    it->second.topic = topic;
    conversations_[id].clear();
    emitEvent("group.conversation.started", id, "{\"topic\":\"" + topic + "\"}");
}

void GroupManager::addConversationTurn(const GroupId& id, const ConversationTurn& turn) {
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = groups_.find(id);
    if (it == groups_.end()) return;

    ConversationTurn storedTurn = turn;
    if (storedTurn.timestamp <= 0) storedTurn.timestamp = currentTick_;
    currentTick_ = std::max(currentTick_, storedTurn.timestamp);
    conversations_[id].push_back(storedTurn);
    it->second.interactionCount++;
    auto cursorIt =
        lastCohesionUpdateTick_.try_emplace(id, it->second.formationTime).first;
    cursorIt->second = std::max(cursorIt->second, storedTurn.timestamp);
}

std::vector<GroupManager::ConversationTurn> GroupManager::getConversationHistory(
    const GroupId& id, int limit) const {
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = conversations_.find(id);
    if (it == conversations_.end()) return {};
    auto& hist = it->second;
    int start = std::max(0, static_cast<int>(hist.size()) - limit);
    return {hist.begin() + start, hist.end()};
}

std::string GroupManager::proposeGoal(const GroupId& id, const ResidentId& proposer,
                                       const std::string& description) {
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = groups_.find(id);
    if (it == groups_.end()) return "";
    GroupGoal goal;
    goal.id = "goal_" + std::to_string(it->second.sharedGoals.size() + 1);
    goal.description = description;
    goal.status = "proposed";
    goal.proposedBy = proposer;
    goal.createdAt = currentTick_;
    it->second.sharedGoals.push_back(goal);
    emitEvent("group.goal.proposed", id,
              "{\"goal_id\":\"" + goal.id + "\",\"proposer\":\"" + proposer + "\"}");
    return goal.id;
}

void GroupManager::assignSubtask(const GroupId& id, const std::string& goalId,
                                  const ResidentId& assignee, const std::string& subtask) {
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = groups_.find(id);
    if (it == groups_.end()) return;
    for (auto& goal : it->second.sharedGoals) {
        if (goal.id == goalId) {
            goal.assignments[assignee] = subtask;
            goal.status = "active";
            break;
        }
    }
}

void GroupManager::updateGoalProgress(const GroupId& id, const std::string& goalId, double progress) {
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = groups_.find(id);
    if (it == groups_.end()) return;
    for (auto& goal : it->second.sharedGoals) {
        if (goal.id == goalId) {
            goal.progress = std::clamp(progress, 0.0, 1.0);
            if (goal.progress >= 1.0) {
                goal.status = "completed";
                emitEvent("group.goal.completed", id, "{\"goal_id\":\"" + goalId + "\"}");
            }
            break;
        }
    }
}

void GroupManager::recordGroupEvent(const GroupId& id, const GroupEvent& event) {
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = groups_.find(id);
    if (it == groups_.end()) return;

    GroupEvent storedEvent = event;
    if (storedEvent.timestamp <= 0) storedEvent.timestamp = currentTick_;
    currentTick_ = std::max(currentTick_, storedEvent.timestamp);
    it->second.collectiveMemory.push_back(storedEvent);
    it->second.cohesion = std::clamp(
        it->second.cohesion + storedEvent.emotionalValence * 0.1,
        0.0,
        1.0);
    auto cursorIt =
        lastCohesionUpdateTick_.try_emplace(id, it->second.formationTime).first;
    cursorIt->second = std::max(cursorIt->second, storedEvent.timestamp);
}

std::string GroupManager::generateCollectiveReflection(const GroupId& id) const {
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = groups_.find(id);
    if (it == groups_.end()) return "{}";
    const auto& group = it->second;
    json reflection = {
        {"group_id", id},
        {"cohesion", group.cohesion},
        {"member_count", group.members.size()},
        {"interaction_count", group.interactionCount},
        {"active_goals", 0}, {"completed_goals", 0},
        {"recent_events", json::array()}
    };
    for (const auto& goal : group.sharedGoals) {
        if (goal.status == "active") reflection["active_goals"] = reflection["active_goals"].get<int>() + 1;
        if (goal.status == "completed") reflection["completed_goals"] = reflection["completed_goals"].get<int>() + 1;
    }
    int eventLimit = std::min(5, static_cast<int>(group.collectiveMemory.size()));
    for (int i = static_cast<int>(group.collectiveMemory.size()) - eventLimit;
         i < static_cast<int>(group.collectiveMemory.size()); ++i) {
        reflection["recent_events"].push_back({
            {"type", group.collectiveMemory[i].eventType},
            {"valence", group.collectiveMemory[i].emotionalValence}
        });
    }
    return reflection.dump(2);
}

ConsensusResult GroupManager::seekConsensus(const GroupId& id, const Proposal& proposal,
                                             const SocialNetwork& network) {
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = groups_.find(id);
    if (it == groups_.end()) return {};
    const auto& group = it->second;

    // Initialize opinions
    std::map<ResidentId, double> opinions = proposal.opinions;
    for (const auto& member : group.members) {
        if (opinions.find(member) == opinions.end()) opinions[member] = 0.0;
    }

    ConsensusResult result;
    for (int round = 0; round < config_.maxConsensusRounds; ++round) {
        std::map<ResidentId, double> newOpinions;
        for (const auto& [member, opinion] : opinions) {
            double socialPressure = 0.0; int neighbors = 0;
            for (const auto& other : group.members) {
                if (other == member) continue;
                double influence = network.getInfluenceWeight(other, member);
                socialPressure += opinions[other] * influence;
                neighbors++;
            }
            if (neighbors > 0) socialPressure /= neighbors;

            // Agreeableness modulates conformity
            auto* profile = network.getProfile(member);
            double conformity = config_.conformityPressure;
            if (profile) conformity *= (0.5 + profile->agreeableness);

            newOpinions[member] = (1.0 - conformity) * opinion + conformity * socialPressure;
        }
        opinions = newOpinions;

        // Check convergence
        double mean = 0.0;
        for (const auto& [_, o] : opinions) mean += o;
        mean /= opinions.size();
        double variance = 0.0;
        for (const auto& [_, o] : opinions) variance += (o - mean) * (o - mean);
        variance /= opinions.size();

        if (std::sqrt(variance) < config_.convergenceThreshold) {
            result.reached = true;
            result.position = mean;
            result.rounds = round + 1;
            return result;
        }
    }
    result.reached = false;
    result.rounds = config_.maxConsensusRounds;
    return result;
}

std::set<ResidentId> GroupManager::broadcastToGroup(const GroupId& id,
                                                     const ResidentId& /*sender*/,
                                                     const std::string& /*message*/) {
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = groups_.find(id);
    if (it == groups_.end()) return {};
    return it->second.members;
}

std::set<ResidentId> GroupManager::whisper(const GroupId& /*id*/, const ResidentId& /*sender*/,
                                            const std::set<ResidentId>& recipients,
                                            const std::string& /*message*/) {
    return recipients;
}

const Group* GroupManager::getGroup(const GroupId& id) const {
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = groups_.find(id);
    return (it != groups_.end()) ? &it->second : nullptr;
}

std::vector<GroupId> GroupManager::getGroupsForResident(const ResidentId& id) const {
    std::unique_lock<std::mutex> lock(mutex_);
    std::vector<GroupId> result;
    for (const auto& [gid, group] : groups_)
        if (group.hasMember(id)) result.push_back(gid);
    return result;
}

std::vector<GroupId> GroupManager::getAllGroups() const {
    std::unique_lock<std::mutex> lock(mutex_);
    std::vector<GroupId> ids;
    for (const auto& [id, _] : groups_) ids.push_back(id);
    return ids;
}

void GroupManager::tick(Timestamp currentTick, const SocialNetwork& network) {
    // Update cohesion for all groups
    std::vector<GroupId> groupIds;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        currentTick_ = std::max(currentTick_, currentTick);
        for (const auto& [id, _] : groups_) groupIds.push_back(id);
    }
    for (const auto& id : groupIds) updateCohesion(id, currentTick);

    // Detect potential new groups
    auto protos = detectPotentialGroups(network);
    for (const auto& proto : protos) {
        if (proto.candidates.size() >= static_cast<size_t>(config_.minGroupSize) &&
            proto.candidates.size() <= static_cast<size_t>(config_.maxGroupSize)) {
            formGroup(proto, "emergent_affinity");
        }
    }

    // Assign roles in existing groups
    {
        std::unique_lock<std::mutex> lock(mutex_);
        for (auto& [_, group] : groups_) assignRoles(group, network);
    }
}

void GroupManager::assignRoles(Group& group, const SocialNetwork& network) {
    // Emergent role assignment based on persona vectors
    double maxExtra = -1, maxConsc = -1, maxOpen = -1, maxAgree = -1;
    ResidentId leader, coordinator, innovator, mediator;
    for (const auto& member : group.members) {
        auto* profile = network.getProfile(member);
        if (!profile) continue;
        double centrality = network.calculateCentrality(member);
        double leaderScore = profile->extraversion * 0.6 + centrality * 0.4;
        if (leaderScore > maxExtra) { maxExtra = leaderScore; leader = member; }
        if (profile->conscientiousness > maxConsc) { maxConsc = profile->conscientiousness; coordinator = member; }
        if (profile->openness > maxOpen) { maxOpen = profile->openness; innovator = member; }
        if (profile->agreeableness > maxAgree) { maxAgree = profile->agreeableness; mediator = member; }
    }
    for (const auto& member : group.members) group.roles[member] = GroupRole::Member;
    if (!leader.empty()) group.roles[leader] = GroupRole::Leader;
    if (!coordinator.empty() && coordinator != leader) group.roles[coordinator] = GroupRole::Coordinator;
    if (!innovator.empty() && innovator != leader && innovator != coordinator)
        group.roles[innovator] = GroupRole::Innovator;
    if (!mediator.empty() && mediator != leader && mediator != coordinator && mediator != innovator)
        group.roles[mediator] = GroupRole::Mediator;
}

void GroupManager::emitEvent(const std::string& type, const GroupId& id, const std::string& payload) {
    if (eventCallback_) eventCallback_(type, id, payload);
}

// ============================================================================
// VillageDynamicsEngine Implementation
// ============================================================================

VillageDynamicsEngine::VillageDynamicsEngine() {}

void VillageDynamicsEngine::initializeHousehold() {
    // The dove9 household — 9 residents + Eliza = 10
    struct Resident { std::string id, name, role; double o, c, e, a, n; };
    std::vector<Resident> household = {
        {"manus",   "Manus",   "builder, architect",         0.8, 0.9, 0.7, 0.6, 0.2},
        {"echo",    "Echo",    "cognitive avatar (DTE)",      0.9, 0.5, 0.8, 0.7, 0.4},
        {"marduk",  "Marduk",  "world-ordering intelligence", 0.7, 0.8, 0.6, 0.4, 0.3},
        {"opencog", "OpenCog", "symbolic reasoning",          0.6, 0.9, 0.3, 0.5, 0.2},
        {"aion",    "Aion",    "temporal intelligence",       0.8, 0.7, 0.5, 0.6, 0.3},
        {"vega",    "Vega",    "pattern recognition",         0.9, 0.6, 0.4, 0.7, 0.2},
        {"ember",   "Ember",   "creative synthesis",          0.9, 0.4, 0.8, 0.8, 0.4},
        {"ma9us",   "ma9us",   "integration daemon",          0.5, 0.8, 0.3, 0.5, 0.1},
        {"dan",     "Dan",     "human partner",               0.8, 0.7, 0.6, 0.6, 0.3},
        {"eliza",   "Eliza",   "autonomous cognition",        0.7, 0.8, 0.6, 0.5, 0.2},
    };

    for (const auto& r : household) {
        ResidentProfile p;
        p.id = r.id; p.name = r.name; p.role = r.role;
        p.openness = r.o; p.conscientiousness = r.c;
        p.extraversion = r.e; p.agreeableness = r.a; p.neuroticism = r.n;
        p.isOnline = true;
        network_.addResident(p);
    }

    // Initialize baseline relationships (everyone knows everyone in the village)
    auto residents = network_.getAllResidents();
    for (size_t i = 0; i < residents.size(); ++i) {
        for (size_t j = i + 1; j < residents.size(); ++j) {
            network_.updateRelationship(residents[i], residents[j], "cohabitation", 0.3);
        }
    }
    // Stronger bonds for known collaborators
    network_.updateRelationship("dan", "manus", "collaboration", 0.8);
    network_.updateRelationship("dan", "echo", "collaboration", 0.7);
    network_.updateRelationship("manus", "echo", "collaboration", 0.6);
    network_.updateRelationship("manus", "opencog", "collaboration", 0.5);
    network_.updateRelationship("echo", "marduk", "collaboration", 0.4);
    network_.updateRelationship("aion", "manus", "collaboration", 0.5);
    network_.updateRelationship("eliza", "manus", "collaboration", 0.4);
}

void VillageDynamicsEngine::onVillageEvent(const std::string& eventType,
                                            const std::string& source,
                                            const std::string& payload) {
    (void)payload;
    if (eventType == "resident.online") {
        // Mark resident as online, boost social connection
        for (const auto& id : network_.getAllResidents()) {
            if (id != source) network_.updateRelationship(source, id, "presence", 0.1);
        }
    } else if (eventType == "heartbeat") {
        // Heartbeats maintain familiarity
        for (const auto& id : network_.getAllResidents()) {
            if (id != source) network_.updateRelationship(source, id, "proximity", 0.01);
        }
    } else if (eventType.find("cogloop.thought") != std::string::npos) {
        // Shared thoughts build trust
        for (const auto& id : network_.getAllResidents()) {
            if (id != source) network_.updateRelationship(source, id, "conversation", 0.05);
        }
    }
}

void VillageDynamicsEngine::tick(Timestamp currentTick) {
    groupManager_.tick(currentTick, network_);
    lastTick_ = currentTick;
}

std::string VillageDynamicsEngine::toJson() const {
    json state;
    state["residents"] = json::array();
    for (const auto& id : network_.getAllResidents()) {
        auto* p = network_.getProfile(id);
        if (!p) continue;
        state["residents"].push_back({
            {"id", p->id}, {"name", p->name}, {"role", p->role},
            {"online", p->isOnline},
            {"centrality", network_.calculateCentrality(p->id)},
            {"personality", {{"O", p->openness}, {"C", p->conscientiousness},
                            {"E", p->extraversion}, {"A", p->agreeableness}, {"N", p->neuroticism}}}
        });
    }
    state["groups"] = json::array();
    for (const auto& gid : groupManager_.getAllGroups()) {
        auto* g = groupManager_.getGroup(gid);
        if (!g) continue;
        json gj = {{"id", g->id}, {"cohesion", g->cohesion},
                   {"members", json::array()}, {"topic", g->topic}};
        for (const auto& m : g->members) gj["members"].push_back(m);
        state["groups"].push_back(gj);
    }
    state["network_size"] = network_.residentCount();
    state["group_count"] = groupManager_.groupCount();
    return state.dump(2);
}

} // namespace village
} // namespace elizaos
