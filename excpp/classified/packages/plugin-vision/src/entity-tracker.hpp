#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class EntityTracker {
public:
    EntityTracker(const std::string& worldId);
    std::future<std::vector<TrackedEntity>> updateEntities(const std::vector<DetectedObject>& detectedObjects, const std::vector<PersonInfo>& people, std::optional<std::unordered_map<std::string, std::string>> faceProfiles);
    std::future<TrackedEntity> trackPerson(PersonInfo person, const std::string& faceProfileId, double timestamp);
    std::future<TrackedEntity> trackObject(DetectedObject obj, double timestamp);
    std::optional<TrackedEntity> findMatchingEntity(BoundingBox boundingBox, const std::string& entityType, std::optional<std::string> faceProfileId);
    double calculateDistance(BoundingBox box1, BoundingBox box2);
    void updateWorldState(const std::unordered_set<std::string>& seenEntityIds, double timestamp);
    std::future<void> syncWithRuntime(IAgentRuntime runtime, const std::vector<TrackedEntity>& frameEntities);
    WorldState getWorldState();
    std::vector<TrackedEntity> getActiveEntities();
    TrackedEntity getEntity(const std::string& entityId);
    Array< getRecentlyLeft();
    bool assignNameToEntity(const std::string& entityId, const std::string& name);
     getStatistics();

private:
    WorldState worldState_;
};


} // namespace elizaos
