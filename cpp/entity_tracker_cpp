#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-vision/src/entity-tracker.h"

EntityTracker::EntityTracker(string worldId) {
    this->worldState = object{
        object::pair{std::string("worldId"), std::string("worldId")}, 
        object::pair{std::string("entities"), std::make_shared<Map>()}, 
        object::pair{std::string("lastUpdate"), Date->now()}, 
        object::pair{std::string("activeEntities"), array<any>()}, 
        object::pair{std::string("recentlyLeft"), array<any>()}
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
        if (AND((obj->type != std::string("person")), (obj->type != std::string("person-candidate")))) {
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
    auto matchedEntity = this->findMatchingEntity(person->boundingBox, std::string("person"), faceProfileId);
    if (matchedEntity) {
        matchedEntity->lastSeen = timestamp;
        matchedEntity->lastPosition = person->boundingBox;
        matchedEntity->appearances->push(object{
            object::pair{std::string("timestamp"), std::string("timestamp")}, 
            object::pair{std::string("boundingBox"), person->boundingBox}, 
            object::pair{std::string("confidence"), person->confidence}
        });
        if (AND((faceProfileId), (!matchedEntity->attributes->faceId))) {
            matchedEntity->attributes->faceId = faceProfileId;
        }
        if (matchedEntity->appearances->get_length() > 100) {
            matchedEntity->appearances = matchedEntity->appearances->slice(-100);
        }
        return matchedEntity;
    } else {
        auto entityId = std::string("person-") + timestamp + std::string("-") + Math->random()->toString(36)->substr(2, 9) + string_empty;
        auto newEntity = object{
            object::pair{std::string("id"), entityId}, 
            object::pair{std::string("entityType"), std::string("person")}, 
            object::pair{std::string("firstSeen"), timestamp}, 
            object::pair{std::string("lastSeen"), timestamp}, 
            object::pair{std::string("lastPosition"), person->boundingBox}, 
            object::pair{std::string("appearances"), array<object>{ object{
                object::pair{std::string("timestamp"), std::string("timestamp")}, 
                object::pair{std::string("boundingBox"), person->boundingBox}, 
                object::pair{std::string("confidence"), person->confidence}
            } }}, 
            object::pair{std::string("attributes"), object{
                object::pair{std::string("faceId"), faceProfileId}
            }}, 
            object::pair{std::string("worldId"), this->worldState->worldId}
        };
        this->worldState->entities->set(entityId, newEntity);
        logger->info(std::string("[EntityTracker] New person entity created: ") + entityId + string_empty);
        return newEntity;
    }
}

std::shared_ptr<Promise<std::shared_ptr<TrackedEntity>>> EntityTracker::trackObject(std::shared_ptr<DetectedObject> obj, double timestamp)
{
    auto matchedEntity = this->findMatchingEntity(obj->boundingBox, std::string("object"));
    if (matchedEntity) {
        matchedEntity->lastSeen = timestamp;
        matchedEntity->lastPosition = obj->boundingBox;
        matchedEntity->appearances->push(object{
            object::pair{std::string("timestamp"), std::string("timestamp")}, 
            object::pair{std::string("boundingBox"), obj->boundingBox}, 
            object::pair{std::string("confidence"), obj->confidence}
        });
        if (matchedEntity->appearances->get_length() > 50) {
            matchedEntity->appearances = matchedEntity->appearances->slice(-50);
        }
        return matchedEntity;
    } else {
        auto entityId = std::string("object-") + timestamp + std::string("-") + Math->random()->toString(36)->substr(2, 9) + string_empty;
        auto newEntity = object{
            object::pair{std::string("id"), entityId}, 
            object::pair{std::string("entityType"), std::string("object")}, 
            object::pair{std::string("firstSeen"), timestamp}, 
            object::pair{std::string("lastSeen"), timestamp}, 
            object::pair{std::string("lastPosition"), obj->boundingBox}, 
            object::pair{std::string("appearances"), array<object>{ object{
                object::pair{std::string("timestamp"), std::string("timestamp")}, 
                object::pair{std::string("boundingBox"), obj->boundingBox}, 
                object::pair{std::string("confidence"), obj->confidence}
            } }}, 
            object::pair{std::string("attributes"), object{
                object::pair{std::string("objectType"), obj->type}
            }}, 
            object::pair{std::string("worldId"), this->worldState->worldId}
        };
        this->worldState->entities->set(entityId, newEntity);
        logger->debug(std::string("[EntityTracker] New object entity created: ") + entityId + std::string(" (") + obj->type + std::string(")"));
        return newEntity;
    }
}

double EntityTracker::calculateDistance(std::shared_ptr<BoundingBox> box1, std::shared_ptr<BoundingBox> box2)
{
    auto center1 = object{
        object::pair{std::string("x"), box1->x + box1->width / 2}, 
        object::pair{std::string("y"), box1->y + box1->height / 2}
    };
    auto center2 = object{
        object::pair{std::string("x"), box2->x + box2->width / 2}, 
        object::pair{std::string("y"), box2->y + box2->height / 2}
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
                object::pair{std::string("entityId"), std::string("entityId")}, 
                object::pair{std::string("leftAt"), timestamp}, 
                object::pair{std::string("lastPosition"), entity["lastPosition"]}
            });
            logger->info(std::string("[EntityTracker] Entity left scene: ") + entityId + string_empty);
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
            logger->debug(std::string("[EntityTracker] Cleaned up old entity: ") + entityId + string_empty);
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
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(entity->id)}, 
                object::pair{std::string("names"), array<string>{ OR((entity->attributes->name), (entity->id)) }}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("type"), entity->entityType}, 
                    object::pair{std::string("firstSeen"), entity->firstSeen}, 
                    object::pair{std::string("lastSeen"), entity->lastSeen}, 
                    object::pair{std::string("attributes"), entity->attributes}, 
                    object::pair{std::string("worldId"), this->worldState->worldId}
                }}, 
                object::pair{std::string("agentId"), runtime->agentId}
            };
            logger->debug(std::string("[EntityTracker] Would sync entity ") + entity->id + std::string(" with runtime"));
        }
    }
    catch (const any& error)
    {
        logger->error(std::string("[EntityTracker] Failed to sync with runtime:"), error);
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
            object::pair{std::string("entity"), this->worldState->entities->get(entry["entityId"])}, 
            object::pair{std::string("leftAt"), entry["leftAt"]}
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
        logger->info(std::string("[EntityTracker] Assigned name "") + name + std::string("" to entity ") + entityId + string_empty);
        return true;
    }
    return false;
}

object EntityTracker::getStatistics()
{
    auto entities = Array->from(this->worldState->entities->values());
    return object{
        object::pair{std::string("totalEntities"), entities->get_length()}, 
        object::pair{std::string("activeEntities"), this->worldState->activeEntities->get_length()}, 
        object::pair{std::string("recentlyLeft"), this->worldState->recentlyLeft->get_length()}, 
        object::pair{std::string("people"), entities->filter([=](auto e) mutable
        {
            return e->entityType == std::string("person");
        }
        )->get_length()}, 
        object::pair{std::string("objects"), entities->filter([=](auto e) mutable
        {
            return e->entityType == std::string("object");
        }
        )->get_length()}
    };
}

