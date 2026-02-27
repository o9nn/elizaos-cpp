#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-vision/src/entity-tracker.h"

EntityTracker::EntityTracker(string worldId) {
    this->worldState = object{
        object::pair{std:("worldId"), std:("worldId")}, 
        object::pair{std:("entities"), std::make_shared<Map>()}, 
        object::pair{std:("lastUpdate"), Date->now()}, 
        object::pair{std:("activeEntities"), array<any>()}, 
        object::pair{std:("recentlyLeft"), array<any>()}
    };
}

std::shared_ptr<Promise<array<std::shared_ptr<TrackedEntity>>>> EntityTracker::updateEntities(array<std::shared_ptr<DetectedObject>> detectedObjects, array<std::shared_ptr<PersonInfo>> people, std::shared_ptr<Map<string, string>> faceProfiles, std::shared_ptr<IAgentRuntime> runtime)
{
    auto currentTime = Date->now();
    auto frameEntities = array<std::shared_ptr<TrackedEntity>>();
    auto seenEntityIds = std::make_shared<Set<string>>();
    for (auto& person : people)
    {
        auto entity = std::async([=]() { this->trackPerson(person, faceProfiles->get(person->id), currentTime); });
        frameEntities->push(entity);
        seenEntityIds->add(entity->id);
    }
    for (auto& obj : detectedObjects)
    {
        if (AND((obj->type != std:("person")), (obj->type != std:("person-candidate")))) {
            auto entity = std::async([=]() { this->trackObject(obj, currentTime); });
            frameEntities->push(entity);
            seenEntityIds->add(entity->id);
        }
    }
    this->updateWorldState(seenEntityIds, currentTime);
    if (runtime) {
        std::async([=]() { this->syncWithRuntime(runtime, frameEntities); });
    }
    return frameEntities;
}

std::shared_ptr<Promise<std::shared_ptr<TrackedEntity>>> EntityTracker::trackPerson(std::shared_ptr<PersonInfo> person, any faceProfileId, double timestamp)
{
    auto matchedEntity = this->findMatchingEntity(person->boundingBox, std:("person"), faceProfileId);
    if (matchedEntity) {
        matchedEntity->lastSeen = timestamp;
        matchedEntity->lastPosition = person->boundingBox;
        matchedEntity->appearances->push(object{
            object::pair{std:("timestamp"), std:("timestamp")}, 
            object::pair{std:("boundingBox"), person->boundingBox}, 
            object::pair{std:("confidence"), person->confidence}
        });
        if (AND((faceProfileId), (!matchedEntity->attributes->faceId))) {
            matchedEntity->attributes->faceId = faceProfileId;
        }
        if (matchedEntity->appearances->get_length() > 100) {
            matchedEntity->appearances = matchedEntity->appearances->slice(-100);
        }
        return matchedEntity;
    } else {
        auto entityId = std:("person-") + timestamp + std:("-") + Math->random()->toString(36)->substr(2, 9) + string_empty;
        auto newEntity = object{
            object::pair{std:("id"), entityId}, 
            object::pair{std:("entityType"), std:("person")}, 
            object::pair{std:("firstSeen"), timestamp}, 
            object::pair{std:("lastSeen"), timestamp}, 
            object::pair{std:("lastPosition"), person->boundingBox}, 
            object::pair{std:("appearances"), array<object>{ object{
                object::pair{std:("timestamp"), std:("timestamp")}, 
                object::pair{std:("boundingBox"), person->boundingBox}, 
                object::pair{std:("confidence"), person->confidence}
            } }}, 
            object::pair{std:("attributes"), object{
                object::pair{std:("faceId"), faceProfileId}
            }}, 
            object::pair{std:("worldId"), this->worldState->worldId}
        };
        this->worldState->entities->set(entityId, newEntity);
        logger->info(std:("[EntityTracker] New person entity created: ") + entityId + string_empty);
        return newEntity;
    }
}

std::shared_ptr<Promise<std::shared_ptr<TrackedEntity>>> EntityTracker::trackObject(std::shared_ptr<DetectedObject> obj, double timestamp)
{
    auto matchedEntity = this->findMatchingEntity(obj->boundingBox, std:("object"));
    if (matchedEntity) {
        matchedEntity->lastSeen = timestamp;
        matchedEntity->lastPosition = obj->boundingBox;
        matchedEntity->appearances->push(object{
            object::pair{std:("timestamp"), std:("timestamp")}, 
            object::pair{std:("boundingBox"), obj->boundingBox}, 
            object::pair{std:("confidence"), obj->confidence}
        });
        if (matchedEntity->appearances->get_length() > 50) {
            matchedEntity->appearances = matchedEntity->appearances->slice(-50);
        }
        return matchedEntity;
    } else {
        auto entityId = std:("object-") + timestamp + std:("-") + Math->random()->toString(36)->substr(2, 9) + string_empty;
        auto newEntity = object{
            object::pair{std:("id"), entityId}, 
            object::pair{std:("entityType"), std:("object")}, 
            object::pair{std:("firstSeen"), timestamp}, 
            object::pair{std:("lastSeen"), timestamp}, 
            object::pair{std:("lastPosition"), obj->boundingBox}, 
            object::pair{std:("appearances"), array<object>{ object{
                object::pair{std:("timestamp"), std:("timestamp")}, 
                object::pair{std:("boundingBox"), obj->boundingBox}, 
                object::pair{std:("confidence"), obj->confidence}
            } }}, 
            object::pair{std:("attributes"), object{
                object::pair{std:("objectType"), obj->type}
            }}, 
            object::pair{std:("worldId"), this->worldState->worldId}
        };
        this->worldState->entities->set(entityId, newEntity);
        logger->debug(std:("[EntityTracker] New object entity created: ") + entityId + std:(" (") + obj->type + std:(")"));
        return newEntity;
    }
}

double EntityTracker::calculateDistance(std::shared_ptr<BoundingBox> box1, std::shared_ptr<BoundingBox> box2)
{
    auto center1 = object{
        object::pair{std:("x"), box1->x + box1->width / 2}, 
        object::pair{std:("y"), box1->y + box1->height / 2}
    };
    auto center2 = object{
        object::pair{std:("x"), box2->x + box2->width / 2}, 
        object::pair{std:("y"), box2->y + box2->height / 2}
    };
    return Math->sqrt(Math->pow(center1["x"] - center2["x"], 2) + Math->pow(center1["y"] - center2["y"], 2));
}

void EntityTracker::updateWorldState(std::shared_ptr<Set<string>> seenEntityIds, double timestamp)
{
    this->worldState->activeEntities = Array->from(seenEntityIds);
    this->worldState->lastUpdate = timestamp;
    for (auto& [entityId, entity] : this->worldState->entities)
    {
        if (AND((!seenEntityIds->has(entityId)), (this->worldState->activeEntities->includes(entityId)))) {
            this->worldState->recentlyLeft->push(object{
                object::pair{std:("entityId"), std:("entityId")}, 
                object::pair{std:("leftAt"), timestamp}, 
                object::pair{std:("lastPosition"), entity["lastPosition"]}
            });
            logger->info(std:("[EntityTracker] Entity left scene: ") + entityId + string_empty);
        }
    }
    this->worldState->recentlyLeft = this->worldState->recentlyLeft->filter([=](auto entry) mutable
    {
        return timestamp - entry["leftAt"] < this->CLEANUP_THRESHOLD;
    }
    );
    for (auto& [entityId, entity] : this->worldState->entities)
    {
        if (timestamp - entity["lastSeen"] > this->CLEANUP_THRESHOLD * 10) {
            this->worldState->entities->delete(entityId);
            logger->debug(std:("[EntityTracker] Cleaned up old entity: ") + entityId + string_empty);
        }
    }
}

std::shared_ptr<Promise<void>> EntityTracker::syncWithRuntime(std::shared_ptr<IAgentRuntime> runtime, array<std::shared_ptr<TrackedEntity>> frameEntities)
{
    try
    {
        for (auto& entity : frameEntities)
        {
            auto _elizaEntity = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(entity->id)}, 
                object::pair{std:("names"), array<string>{ OR((entity->attributes->name), (entity->id)) }}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("type"), entity->entityType}, 
                    object::pair{std:("firstSeen"), entity->firstSeen}, 
                    object::pair{std:("lastSeen"), entity->lastSeen}, 
                    object::pair{std:("attributes"), entity->attributes}, 
                    object::pair{std:("worldId"), this->worldState->worldId}
                }}, 
                object::pair{std:("agentId"), runtime->agentId}
            };
            logger->debug(std:("[EntityTracker] Would sync entity ") + entity->id + std:(" with runtime"));
        }
    }
    catch (const any& error)
    {
        logger->error(std:("[EntityTracker] Failed to sync with runtime:"), error);
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<WorldState> EntityTracker::getWorldState()
{
    return this->worldState;
}

array<std::shared_ptr<TrackedEntity>> EntityTracker::getActiveEntities()
{
    return as<array<std::shared_ptr<TrackedEntity>>>(this->worldState->activeEntities->map([=](auto id) mutable
    {
        return this->worldState->entities->get(id);
    }
    )->filter(Boolean));
}

any EntityTracker::getEntity(string entityId)
{
    return this->worldState->entities->get(entityId);
}

array<object> EntityTracker::getRecentlyLeft()
{
    return as<array<object>>(this->worldState->recentlyLeft->map([=](auto entry) mutable
    {
        return (object{
            object::pair{std:("entity"), this->worldState->entities->get(entry["entityId"])}, 
            object::pair{std:("leftAt"), entry["leftAt"]}
        });
    }
    )->filter([=](auto entry) mutable
    {
        return entry["entity"];
    }
    ));
}

boolean EntityTracker::assignNameToEntity(string entityId, string name)
{
    auto entity = this->worldState->entities->get(entityId);
    if (entity) {
        entity->attributes->name = name;
        logger->info(std:("[EntityTracker] Assigned name "") + name + std:("" to entity ") + entityId + string_empty);
        return true;
    }
    return false;
}

object EntityTracker::getStatistics()
{
    auto entities = Array->from(this->worldState->entities->values());
    return object{
        object::pair{std:("totalEntities"), entities->get_length()}, 
        object::pair{std:("activeEntities"), this->worldState->activeEntities->get_length()}, 
        object::pair{std:("recentlyLeft"), this->worldState->recentlyLeft->get_length()}, 
        object::pair{std:("people"), entities->filter([=](auto e) mutable
        {
            return e->entityType == std:("person");
        }
        )->get_length()}, 
        object::pair{std:("objects"), entities->filter([=](auto e) mutable
        {
            return e->entityType == std:("object");
        }
        )->get_length()}
    };
}

