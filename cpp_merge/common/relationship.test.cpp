#include "relationship.test.h"

std::function<any(double)> createTestUUID = [=](auto num) mutable
{
    return std:("00000000-0000-0000-0000-") + num->toString()->padStart(12, std:("0")) + string_empty;
};

void Main(void)
{
    describe(std:("Relationship adapter"), [=]() mutable
    {
        shared testAgentId = createTestUUID(1);
        shared testUserA = createTestUUID(2);
        shared testUserB = createTestUUID(3);
        shared testRelId = createTestUUID(4);
        describe(std:("Basic conversion"), [=]() mutable
        {
            it(std:("should convert from v2 relationship to v1 relationship correctly"), [=]() mutable
            {
                auto relationshipV2 = object{
                    object::pair{std:("id"), testRelId}, 
                    object::pair{std:("sourceEntityId"), testUserA}, 
                    object::pair{std:("targetEntityId"), testUserB}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("tags"), array<string>{ std:("friend"), std:("colleague") }}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("strength"), 0.8}, 
                        object::pair{std:("lastContact"), std:("2023-01-01")}
                    }}, 
                    object::pair{std:("createdAt"), std:("2023-01-01T00:00:00Z")}
                };
                auto relationshipV1 = fromV2Relationship(relationshipV2);
                expect(relationshipV1->id)->toBe(testRelId);
                expect(relationshipV1->userA)->toBe(testUserA);
                expect(relationshipV1->userB)->toBe(testUserB);
                expect(relationshipV1->userId)->toBe(testUserA);
                expect(relationshipV1->roomId)->toBe(testRelId);
                expect(relationshipV1->status)->toBe(std:("friend,colleague"));
                expect(relationshipV1->createdAt)->toBe(std:("2023-01-01T00:00:00Z"));
            }
            );
            it(std:("should convert from v1 relationship to v2 relationship correctly"), [=]() mutable
            {
                auto relationshipV1 = object{
                    object::pair{std:("id"), testRelId}, 
                    object::pair{std:("userA"), testUserA}, 
                    object::pair{std:("userB"), testUserB}, 
                    object::pair{std:("userId"), testUserA}, 
                    object::pair{std:("roomId"), createTestUUID(5)}, 
                    object::pair{std:("status"), std:("friend,blocked")}, 
                    object::pair{std:("createdAt"), std:("2023-01-01T00:00:00Z")}
                };
                auto relationshipV2 = toV2Relationship(relationshipV1, testAgentId);
                expect(relationshipV2->id)->toBe(testRelId);
                expect(relationshipV2->sourceEntityId)->toBe(testUserA);
                expect(relationshipV2->targetEntityId)->toBe(testUserB);
                expect(relationshipV2->agentId)->toBe(testAgentId);
                expect(relationshipV2->tags)->toEqual(array<string>{ std:("friend"), std:("blocked") });
                expect(relationshipV2->metadata->userId)->toBe(testUserA);
                expect(relationshipV2->metadata->roomId)->toBe(createTestUUID(5));
                expect(relationshipV2->createdAt)->toBe(std:("2023-01-01T00:00:00Z"));
            }
            );
        }
        );
        describe(std:("Enhanced conversion with status mapping"), [=]() mutable
        {
            it(std:("should map common V2 tags to V1 statuses"), [=]() mutable
            {
                expect(tagsToStatus(array<string>{ std:("friend") }))->toBe(RELATIONSHIP_STATUSES["FRIEND"]);
                expect(tagsToStatus(array<string>{ std:("blocked") }))->toBe(RELATIONSHIP_STATUSES["BLOCKED"]);
                expect(tagsToStatus(array<string>{ std:("muted") }))->toBe(RELATIONSHIP_STATUSES["MUTED"]);
                expect(tagsToStatus(array<string>{ std:("following") }))->toBe(RELATIONSHIP_STATUSES["FOLLOWING"]);
                expect(tagsToStatus(array<string>{ std:("follower") }))->toBe(RELATIONSHIP_STATUSES["FOLLOWED_BY"]);
                expect(tagsToStatus(array<string>{ std:("acquaintance") }))->toBe(RELATIONSHIP_STATUSES["ACQUAINTANCE"]);
            }
            );
            it(std:("should map common V1 statuses to V2 tags"), [=]() mutable
            {
                expect(statusToTags(RELATIONSHIP_STATUSES["FRIEND"]))->toEqual(array<string>{ std:("friend") });
                expect(statusToTags(RELATIONSHIP_STATUSES["BLOCKED"]))->toEqual(array<string>{ std:("blocked") });
                expect(statusToTags(RELATIONSHIP_STATUSES["MUTED"]))->toEqual(array<string>{ std:("muted") });
                expect(statusToTags(RELATIONSHIP_STATUSES["FOLLOWING"]))->toEqual(array<string>{ std:("following") });
                expect(statusToTags(RELATIONSHIP_STATUSES["FOLLOWED_BY"]))->toEqual(array<string>{ std:("follower") });
                expect(statusToTags(RELATIONSHIP_STATUSES["ACQUAINTANCE"]))->toEqual(array<string>{ std:("acquaintance") });
                expect(statusToTags(RELATIONSHIP_STATUSES["UNKNOWN"]))->toEqual(array<any>());
            }
            );
            it(std:("should handle unknown tags and statuses gracefully"), [=]() mutable
            {
                expect(tagsToStatus(array<string>{ std:("custom-tag"), std:("another-tag") }))->toBe(std:("custom-tag,another-tag"));
                expect(statusToTags(std:("custom,status")))->toEqual(array<string>{ std:("custom"), std:("status") });
            }
            );
            it(std:("should use enhanced conversion correctly"), [=]() mutable
            {
                auto relationshipV2 = object{
                    object::pair{std:("id"), testRelId}, 
                    object::pair{std:("sourceEntityId"), testUserA}, 
                    object::pair{std:("targetEntityId"), testUserB}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("tags"), array<string>{ std:("friend") }}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("roomId"), createTestUUID(5)}
                    }}
                };
                auto relationshipV1 = fromV2RelationshipEnhanced(relationshipV2);
                expect(relationshipV1->status)->toBe(RELATIONSHIP_STATUSES["FRIEND"]);
                expect(relationshipV1->roomId)->toBe(createTestUUID(5));
            }
            );
        }
        );
        describe(std:("Utility functions"), [=]() mutable
        {
            it(std:("should create V1 relationship with defaults"), [=]() mutable
            {
                auto relationship = createV1Relationship(testUserA, testUserB);
                expect(relationship->userA)->toBe(testUserA);
                expect(relationship->userB)->toBe(testUserB);
                expect(relationship->userId)->toBe(testUserA);
                expect(relationship->roomId)->toBe(testUserA);
                expect(relationship->status)->toBe(RELATIONSHIP_STATUSES["UNKNOWN"]);
                expect(relationship->id)->toContain(testUserA);
                expect(relationship->createdAt)->toBeDefined();
            }
            );
            it(std:("should identify equivalent relationships"), [=]() mutable
            {
                auto rel1 = object{
                    object::pair{std:("id"), createTestUUID(10)}, 
                    object::pair{std:("userA"), testUserA}, 
                    object::pair{std:("userB"), testUserB}, 
                    object::pair{std:("userId"), testUserA}, 
                    object::pair{std:("roomId"), createTestUUID(5)}, 
                    object::pair{std:("status"), std:("friend")}
                };
                auto rel2 = object{
                    object::pair{std:("id"), createTestUUID(11)}, 
                    object::pair{std:("userA"), testUserB}, 
                    object::pair{std:("userB"), testUserA}, 
                    object::pair{std:("userId"), testUserB}, 
                    object::pair{std:("roomId"), createTestUUID(6)}, 
                    object::pair{std:("status"), std:("colleague")}
                };
                auto rel3 = object{
                    object::pair{std:("id"), createTestUUID(12)}, 
                    object::pair{std:("userA"), testUserA}, 
                    object::pair{std:("userB"), createTestUUID(99)}, 
                    object::pair{std:("userId"), testUserA}, 
                    object::pair{std:("roomId"), createTestUUID(7)}, 
                    object::pair{std:("status"), std:("friend")}
                };
                expect(areRelationshipsEquivalent(rel1, rel2))->toBe(true);
                expect(areRelationshipsEquivalent(rel1, rel3))->toBe(false);
            }
            );
        }
        );
        describe(std:("Round-trip conversion"), [=]() mutable
        {
            it(std:("should preserve key information in round-trip conversion"), [=]() mutable
            {
                auto originalRelationship = object{
                    object::pair{std:("id"), testRelId}, 
                    object::pair{std:("userA"), testUserA}, 
                    object::pair{std:("userB"), testUserB}, 
                    object::pair{std:("userId"), testUserA}, 
                    object::pair{std:("roomId"), createTestUUID(5)}, 
                    object::pair{std:("status"), RELATIONSHIP_STATUSES["FRIEND"]}, 
                    object::pair{std:("createdAt"), std:("2023-01-01T00:00:00Z")}
                };
                auto relationshipV2 = toV2RelationshipEnhanced(originalRelationship, testAgentId);
                auto convertedBack = fromV2RelationshipEnhanced(relationshipV2);
                expect(convertedBack->id)->toBe(originalRelationship->id);
                expect(convertedBack->userA)->toBe(originalRelationship->userA);
                expect(convertedBack->userB)->toBe(originalRelationship->userB);
                expect(convertedBack->userId)->toBe(originalRelationship->userId);
                expect(convertedBack->roomId)->toBe(originalRelationship->roomId);
                expect(convertedBack->status)->toBe(originalRelationship->status);
                expect(convertedBack->createdAt)->toBe(originalRelationship->createdAt);
            }
            );
        }
        );
    }
    );
}

MAIN
