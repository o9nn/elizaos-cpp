#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_VISION_SRC_ENTITY_TRACKER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_VISION_SRC_ENTITY_TRACKER_H
#include "core.hpp"
#include <string>
// External dependency removed
#include "./types.h"

class EntityTracker;

class EntityTracker : public object, public std::enable_shared_from_this<EntityTracker> {
public:
    using std::enable_shared_from_this<EntityTracker>::shared_from_this;
    std::shared_ptr<WorldState> worldState;

    double POSITION_THRESHOLD = 100;

    double MISSING_THRESHOLD = 5000;

    double CLEANUP_THRESHOLD = 60000;

    EntityTracker(string worldId);
    virtual std::shared_ptr<Promise<array<std::shared_ptr<TrackedEntity>>>> updateEntities(array<std::shared_ptr<DetectedObject>> detectedObjects, array<std::shared_ptr<PersonInfo>> people, std::shared_ptr<Map<string, string>> faceProfiles = std::nullopt, std::shared_ptr<IAgentRuntime> runtime = std::nullopt);
    virtual std::shared_ptr<Promise<std::shared_ptr<TrackedEntity>>> trackPerson(std::shared_ptr<PersonInfo> person, any faceProfileId, double timestamp);
    virtual std::shared_ptr<Promise<std::shared_ptr<TrackedEntity>>> trackObject(std::shared_ptr<DetectedObject> obj, double timestamp);
    template <typename P1>
    any findMatchingEntity(std::shared_ptr<BoundingBox> boundingBox, P1 entityType, string faceProfileId = std::nullopt);
    virtual double calculateDistance(std::shared_ptr<BoundingBox> box1, std::shared_ptr<BoundingBox> box2);
    virtual void updateWorldState(std::shared_ptr<Set<string>> seenEntityIds, double timestamp);
    virtual std::shared_ptr<Promise<void>> syncWithRuntime(std::shared_ptr<IAgentRuntime> runtime, array<std::shared_ptr<TrackedEntity>> frameEntities);
    virtual std::shared_ptr<WorldState> getWorldState();
    virtual array<std::shared_ptr<TrackedEntity>> getActiveEntities();
    virtual any getEntity(string entityId);
    virtual array<object> getRecentlyLeft();
    virtual boolean assignNameToEntity(string entityId, string name);
    virtual object getStatistics();
};

template <typename P1>
any EntityTracker::findMatchingEntity(std::shared_ptr<BoundingBox> boundingBox, P1 entityType, string faceProfileId)
{
    auto currentTime = Date->now();
    auto bestMatch = nullptr;
    auto minDistance = Infinity;
    for (auto& entity : this->worldState->entities->values())
    {
        if (entity["entityType"] != entityType) {
            continue;
        }
        if (currentTime - entity["lastSeen"] > this->MISSING_THRESHOLD) {
            continue;
        }
        if (AND((AND((entityType == std::string("person")), (faceProfileId))), (entity["attributes"]["faceId"]))) {
            if (entity["attributes"]["faceId"] == faceProfileId) {
                return entity;
            }
        }
        auto distance = this->calculateDistance(entity["lastPosition"], boundingBox);
        if (AND((distance < this->POSITION_THRESHOLD), (distance < minDistance))) {
            minDistance = distance;
            bestMatch = entity;
        }
    }
    return bestMatch;
}

#endif
