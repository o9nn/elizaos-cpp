#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/core/src/specs/v1/__tests__/state.test.h"

std::function<any(double)> createTestUUID = [=](auto num) mutable
{
    return std:("00000000-0000-0000-0000-") + num->toString()->padStart(12, std:("0")) + string_empty;
};
array<object> emptyMemoryData = array<object>{ object{
    object::pair{std:("id"), createTestUUID(1)}, 
    object::pair{std:("roomId"), createTestUUID(3)}, 
    object::pair{std:("userId"), createTestUUID(4)}, 
    object::pair{std:("agentId"), createTestUUID(5)}, 
    object::pair{std:("content"), object{
        object::pair{std:("text"), std:("Test message")}
    }}
} };

void Main(void)
{
    describe(std:("State adapter"), [=]() mutable
    {
        it(std:("should convert from v2 state to v1 state correctly"), [=]() mutable
        {
            auto stateV2 = object{
                object::pair{std:("values"), object{
                    object::pair{std:("userId"), createTestUUID(123)}, 
                    object::pair{std:("agentName"), std:("TestAgent")}
                }}, 
                object::pair{std:("data"), object{
                    object::pair{std:("walletBalance"), 100}, 
                    object::pair{std:("tokenPrices"), object{
                        object::pair{std:("ETH"), 2000}
                    }}
                }}, 
                object::pair{std:("text"), std:("Current state information")}
            };
            auto stateV1 = fromV2State(stateV2);
            expect(stateV1->userId)->toBe(createTestUUID(123));
            expect(stateV1->agentName)->toBe(std:("TestAgent"));
            expect(stateV1->walletBalance)->toBe(100);
            expect(stateV1->tokenPrices)->toEqual(object{
                object::pair{std:("ETH"), 2000}
            });
            expect(stateV1->text)->toBe(std:("Current state information"));
            expect(stateV1->bio)->toBe(string_empty);
            expect(stateV1->lore)->toBe(string_empty);
            expect(stateV1->messageDirections)->toBe(string_empty);
            expect(stateV1->postDirections)->toBe(string_empty);
            expect(stateV1->recentMessagesData)->toEqual(array<any>());
            expect(stateV1->actors)->toBe(string_empty);
        }
        );
        it(std:("should convert from v1 state to v2 state correctly"), [=]() mutable
        {
            auto stateV1 = object{
                object::pair{std:("userId"), createTestUUID(123)}, 
                object::pair{std:("agentName"), std:("TestAgent")}, 
                object::pair{std:("walletBalance"), 100}, 
                object::pair{std:("tokenPrices"), object{
                    object::pair{std:("ETH"), 2000}
                }}, 
                object::pair{std:("text"), std:("Current state information")}, 
                object::pair{std:("recentMessages"), std:("Some recent messages")}, 
                object::pair{std:("recentMessagesData"), emptyMemoryData}, 
                object::pair{std:("bio"), std:("Agent bio")}, 
                object::pair{std:("lore"), std:("Agent lore")}, 
                object::pair{std:("messageDirections"), std:("Handle messages this way")}, 
                object::pair{std:("postDirections"), std:("Handle posts this way")}, 
                object::pair{std:("roomId"), createTestUUID(456)}, 
                object::pair{std:("actors"), std:("User, Agent")}
            };
            auto stateV2 = toV2State(stateV1);
            expect(stateV2->values)->toBeDefined();
            expect(stateV2->data)->toBeDefined();
            expect(stateV2->text)->toBe(std:("Current state information"));
            expect(stateV2->userId)->toBe(createTestUUID(123));
            expect(stateV2->agentName)->toBe(std:("TestAgent"));
            expect(stateV2->bio)->toBe(std:("Agent bio"));
            expect(stateV2->lore)->toBe(std:("Agent lore"));
            expect(stateV2->recentMessagesData)->toEqual(emptyMemoryData);
        }
        );
        it(std:("should handle empty or undefined values"), [=]() mutable
        {
            auto emptyV2 = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            auto emptyV1 = fromV2State(emptyV2);
            auto backToV2 = toV2State(emptyV1);
            expect(emptyV1)->toEqual(object{
                object::pair{std:("bio"), string_empty}, 
                object::pair{std:("lore"), string_empty}, 
                object::pair{std:("messageDirections"), string_empty}, 
                object::pair{std:("postDirections"), string_empty}, 
                object::pair{std:("actors"), string_empty}, 
                object::pair{std:("recentMessages"), string_empty}, 
                object::pair{std:("recentMessagesData"), array<any>()}, 
                object::pair{std:("text"), string_empty}
            });
            expect(backToV2)->toEqual(object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}, 
                object::pair{std:("bio"), string_empty}, 
                object::pair{std:("lore"), string_empty}, 
                object::pair{std:("messageDirections"), string_empty}, 
                object::pair{std:("postDirections"), string_empty}, 
                object::pair{std:("actors"), string_empty}, 
                object::pair{std:("recentMessages"), string_empty}, 
                object::pair{std:("recentMessagesData"), array<any>()}
            });
        }
        );
        it(std:("should handle additional properties from real-world plugins"), [=]() mutable
        {
            auto tonStateV1 = object{
                object::pair{std:("userId"), createTestUUID(123)}, 
                object::pair{std:("agentName"), std:("TonBot")}, 
                object::pair{std:("walletAddress"), std:("0x123abc")}, 
                object::pair{std:("walletBalance"), 10.5}, 
                object::pair{std:("stakedAmount"), 5.25}, 
                object::pair{std:("lastTransaction"), std:("2023-04-01")}, 
                object::pair{std:("roomId"), createTestUUID(456)}, 
                object::pair{std:("recentMessages"), std:("Recent messages here")}, 
                object::pair{std:("recentMessagesData"), emptyMemoryData}, 
                object::pair{std:("bio"), std:("TON blockchain assistant")}, 
                object::pair{std:("lore"), std:("Helps with TON transactions")}, 
                object::pair{std:("messageDirections"), std:("Handle DMs from users")}, 
                object::pair{std:("postDirections"), std:("Post updates about TON")}, 
                object::pair{std:("actors"), std:("User, TonBot")}, 
                object::pair{std:("text"), std:("Current state")}
            };
            auto tonStateV2 = toV2State(tonStateV1);
            auto tonStateV1Again = fromV2State(tonStateV2);
            expect(tonStateV1Again->walletAddress)->toBe(std:("0x123abc"));
            expect(tonStateV1Again->walletBalance)->toBe(10.5);
            expect(tonStateV1Again->stakedAmount)->toBe(5.25);
            expect(tonStateV1Again->lastTransaction)->toBe(std:("2023-04-01"));
            expect(tonStateV1Again->recentMessagesData)->toEqual(emptyMemoryData);
        }
        );
    }
    );
}

MAIN
