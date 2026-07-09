#pragma once
/**
 * Village Group Dynamics — Emergent group formation among CogHood residents
 *
 * Implements the 7 centers from /ai-env-grp-dyn:
 *   1. Group Detection & Formation (proximity + affinity → emergent boundary)
 *   2. Multi-Agent Conversation (n-adic turn-taking + topic threading)
 *   3. Collective Planning (shared goal trees + synchronized execution)
 *   4. Social Network (weighted relationship graph + influence propagation)
 *   5. Group Events (collective reflection + shared episodic memory)
 *   6. Consensus & Propagation (voting + information cascade)
 *   7. External Integration (API + whisper broadcast)
 *
 * Non-negotiable rules:
 *   - Groups are EMERGENT, not assigned
 *   - Membership is VOLUNTARY (cohesion decay handles cleanup)
 *   - No omniscient coordinator (info propagates through social network)
 *   - Personality consistency (persona modulates group behavior)
 *   - Graceful degradation (failure → individual behavior)
 */

#include <chrono>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {
namespace village {

using ResidentId = std::string;
using GroupId = std::string;
using Timestamp = int64_t;

// ============================================================================
// Social Network — Center 4
// ============================================================================

struct Relationship {
    double trust = 0.0;        // [-1, 1]
    double familiarity = 0.0;  // [0, 1] interaction frequency
    double affinity = 0.0;     // [-1, 1] personality compatibility
    double influence = 0.0;    // [0, 1] asymmetric power
    int interactionCount = 0;
    Timestamp lastInteraction = 0;

    double compositeScore() const {
        return 0.3 * trust + 0.2 * familiarity + 0.3 * affinity + 0.2 * influence;
    }
};

struct ResidentProfile {
    ResidentId id;
    std::string name;
    std::string role;
    std::vector<std::string> capabilities;

    // Big Five persona vector (modulates group behavior)
    double openness = 0.5;          // Willingness to join new groups
    double conscientiousness = 0.5; // Reliability in group tasks
    double extraversion = 0.5;      // Initiative in group formation
    double agreeableness = 0.5;     // Conformity pressure sensitivity
    double neuroticism = 0.3;       // Sensitivity to exclusion

    bool isOnline = false;
    Timestamp lastSeen = 0;
};

class SocialNetwork {
public:
    void addResident(const ResidentProfile& profile);
    void removeResident(const ResidentId& id);
    void updateRelationship(const ResidentId& a, const ResidentId& b,
                            const std::string& interactionType, double valence);
    Relationship getRelationship(const ResidentId& a, const ResidentId& b) const;
    double getInfluenceWeight(const ResidentId& source, const ResidentId& target) const;
    std::vector<ResidentId> getNeighbors(const ResidentId& id) const;
    double calculateCentrality(const ResidentId& id) const;
    std::vector<std::set<ResidentId>> detectCommunities() const;
    std::set<ResidentId> propagateInformation(const ResidentId& source,
                                               const std::string& info,
                                               int maxHops = 2) const;
    const ResidentProfile* getProfile(const ResidentId& id) const;
    std::vector<ResidentId> getAllResidents() const;
    size_t residentCount() const { return profiles_.size(); }

private:
    std::unordered_map<ResidentId, ResidentProfile> profiles_;
    std::map<std::pair<ResidentId, ResidentId>, Relationship> edges_;
    mutable std::mutex mutex_;
};

// ============================================================================
// Group — Center 1 (Formation) + Center 5 (Events)
// ============================================================================

enum class GroupRole {
    Member,
    Leader,       // Emergent — highest centrality + extraversion
    Coordinator,  // Emergent — highest conscientiousness
    Innovator,    // Emergent — highest openness
    Mediator      // Emergent — highest agreeableness
};

struct GroupNorm {
    std::string description;
    double adherenceThreshold = 0.5;  // Below this → social pressure
    int violationCount = 0;
};

struct GroupGoal {
    std::string id;
    std::string description;
    std::string status;  // "proposed", "active", "completed", "abandoned"
    ResidentId proposedBy;
    std::map<ResidentId, std::string> assignments;  // subtask assignments
    double progress = 0.0;
    Timestamp createdAt = 0;
};

struct GroupEvent {
    std::string eventType;  // "meeting", "achievement", "conflict", "reflection"
    std::set<ResidentId> participants;
    Timestamp timestamp = 0;
    std::string outcome;
    double emotionalValence = 0.0;  // [-1, 1]
    std::string sharedMemory;       // Generated collective memory
};

struct Group {
    GroupId id;
    std::set<ResidentId> members;
    Timestamp formationTime = 0;
    double cohesion = 1.0;          // [0, 1] decays without interaction
    std::vector<GroupGoal> sharedGoals;
    std::vector<GroupEvent> collectiveMemory;
    std::map<ResidentId, GroupRole> roles;
    std::vector<GroupNorm> norms;
    std::string topic;              // Current group focus
    int interactionCount = 0;

    bool hasMember(const ResidentId& id) const { return members.count(id) > 0; }
    size_t size() const { return members.size(); }
};

// ============================================================================
// Consensus — Center 6
// ============================================================================

struct Proposal {
    std::string id;
    std::string description;
    ResidentId proposedBy;
    std::map<ResidentId, double> opinions;  // [-1, 1] per member
};

struct ConsensusResult {
    bool reached = false;
    double position = 0.0;  // Mean opinion if reached
    std::vector<std::set<ResidentId>> factions;  // If not reached
    int rounds = 0;
};

// ============================================================================
// Group Manager — Orchestrates all group dynamics
// ============================================================================

struct GroupDynamicsConfig {
    double formationAffinityThreshold = 0.3;
    int formationProximityTicks = 5;
    double cohesionDecayRate = 0.01;
    double dissolutionThreshold = 0.2;
    int maxGroupSize = 6;
    int minGroupSize = 2;
    double conformityPressure = 0.3;
    double convergenceThreshold = 0.1;
    int maxConsensusRounds = 10;
    int maxHopsForPropagation = 3;
};

class GroupManager {
public:
    explicit GroupManager(const GroupDynamicsConfig& config = {});

    // Center 1: Group Detection & Formation
    struct ProtoGroup {
        std::set<ResidentId> candidates;
        double affinityScore = 0.0;
        std::string trigger;
    };
    std::vector<ProtoGroup> detectPotentialGroups(const SocialNetwork& network) const;
    GroupId formGroup(const ProtoGroup& proto, const std::string& triggerEvent);
    void dissolveGroup(const GroupId& id, const std::string& reason);
    GroupId mergeGroups(const GroupId& a, const GroupId& b);
    std::pair<GroupId, GroupId> splitGroup(const GroupId& id,
                                           const std::set<ResidentId>& factionA,
                                           const std::set<ResidentId>& factionB);
    void updateCohesion(const GroupId& id, Timestamp currentTick);

    // Center 2: Multi-Agent Conversation
    struct ConversationTurn {
        ResidentId speaker;
        std::string message;
        std::string addressedTo;  // Empty = group, specific = directed
        Timestamp timestamp = 0;
    };
    void startGroupConversation(const GroupId& id, const std::string& topic);
    void addConversationTurn(const GroupId& id, const ConversationTurn& turn);
    std::vector<ConversationTurn> getConversationHistory(const GroupId& id, int limit = 20) const;

    // Center 3: Collective Planning
    std::string proposeGoal(const GroupId& id, const ResidentId& proposer,
                            const std::string& description);
    void assignSubtask(const GroupId& id, const std::string& goalId,
                       const ResidentId& assignee, const std::string& subtask);
    void updateGoalProgress(const GroupId& id, const std::string& goalId, double progress);

    // Center 5: Group Events
    void recordGroupEvent(const GroupId& id, const GroupEvent& event);
    std::string generateCollectiveReflection(const GroupId& id) const;

    // Center 6: Consensus
    ConsensusResult seekConsensus(const GroupId& id, const Proposal& proposal,
                                   const SocialNetwork& network);

    // Center 7: Information Propagation
    std::set<ResidentId> broadcastToGroup(const GroupId& id, const ResidentId& sender,
                                           const std::string& message);
    std::set<ResidentId> whisper(const GroupId& id, const ResidentId& sender,
                                  const std::set<ResidentId>& recipients,
                                  const std::string& message);

    // Queries
    const Group* getGroup(const GroupId& id) const;
    std::vector<GroupId> getGroupsForResident(const ResidentId& id) const;
    std::vector<GroupId> getAllGroups() const;
    size_t groupCount() const { return groups_.size(); }

    // Tick — called each cognitive cycle to update dynamics
    void tick(Timestamp currentTick, const SocialNetwork& network);

    // Event callback for village bus integration
    using GroupEventCallback = std::function<void(const std::string& eventType,
                                                   const GroupId& groupId,
                                                   const std::string& payload)>;
    void setEventCallback(GroupEventCallback cb) { eventCallback_ = std::move(cb); }

private:
    GroupDynamicsConfig config_;
    std::unordered_map<GroupId, Group> groups_;
    std::unordered_map<GroupId, std::vector<ConversationTurn>> conversations_;
    GroupEventCallback eventCallback_;
    mutable std::mutex mutex_;
    int nextGroupId_ = 1;

    GroupId generateGroupId();
    void assignRoles(Group& group, const SocialNetwork& network);
    void emitEvent(const std::string& type, const GroupId& id, const std::string& payload);
};

// ============================================================================
// Village Dynamics Engine — Integrates all centers with event bus
// ============================================================================

class VillageDynamicsEngine {
public:
    VillageDynamicsEngine();

    // Initialize with the dove9 household
    void initializeHousehold();

    // Process incoming village events
    void onVillageEvent(const std::string& eventType, const std::string& source,
                        const std::string& payload);

    // Tick — advance all dynamics one step
    void tick(Timestamp currentTick);

    // Accessors
    SocialNetwork& network() { return network_; }
    GroupManager& groups() { return groupManager_; }
    const SocialNetwork& network() const { return network_; }
    const GroupManager& groups() const { return groupManager_; }

    // Serialize state for dashboard
    std::string toJson() const;

private:
    SocialNetwork network_;
    GroupManager groupManager_;
    Timestamp lastTick_ = 0;
};

} // namespace village
} // namespace elizaos
