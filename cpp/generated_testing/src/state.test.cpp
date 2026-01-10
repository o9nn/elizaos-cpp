#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v1/src/__tests__/state.test.h"

std::function<any(double)> createTestUUID = [=](auto num) mutable
{
    return std::string("00000000-0000-0000-0000-") + num->toString()->padStart(12, std::string("0")) + string_empty;
};
array<object> emptyMemoryData = array<object>{ object{
    object::pair{std::string("id"), createTestUUID(1)}, 
    object::pair{std::string("roomId"), createTestUUID(3)}, 
    object::pair{std::string("userId"), createTestUUID(4)}, 
    object::pair{std::string("agentId"), createTestUUID(5)}, 
    object::pair{std::string("content"), object{
        object::pair{std::string("text"), std::string("Test message")}
    }}
} };

void Main(void)
{
    describe(std::string("State adapter"), [=]() mutable
    {
        it(std::string("should convert from v2 state to v1 state correctly"), [=]() mutable
        {
            auto stateV2 = object{
                object::pair{std::string("values"), object{
                    object::pair{std::string("userId"), createTestUUID(123)}, 
                    object::pair{std::string("agentName"), std::string("TestAgent")}
                }}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("walletBalance"), 100}, 
                    object::pair{std::string("tokenPrices"), object{
                        object::pair{std::string("ETH"), 2000}
                    }}
                }}, 
                object::pair{std::string("text"), std::string("Current state information")}
            };
            auto stateV1 = fromV2State(stateV2);
            expect(stateV1->userId)->toBe(createTestUUID(123));
            expect(stateV1->agentName)->toBe(std::string("TestAgent"));
            expect(stateV1->walletBalance)->toBe(100);
            expect(stateV1->tokenPrices)->toEqual(object{
                object::pair{std::string("ETH"), 2000}
            });
            expect(stateV1->text)->toBe(std::string("Current state information"));
            expect(stateV1->bio)->toBe(string_empty);
            expect(stateV1->lore)->toBe(string_empty);
            expect(stateV1->messageDirections)->toBe(string_empty);
            expect(stateV1->postDirections)->toBe(string_empty);
            expect(stateV1->recentMessagesData)->toEqual(array<any>());
            expect(stateV1->actors)->toBe(string_empty);
        }
        );
        it(std::string("should convert from v1 state to v2 state correctly"), [=]() mutable
        {
            auto stateV1 = object{
                object::pair{std::string("userId"), createTestUUID(123)}, 
                object::pair{std::string("agentName"), std::string("TestAgent")}, 
                object::pair{std::string("walletBalance"), 100}, 
                object::pair{std::string("tokenPrices"), object{
                    object::pair{std::string("ETH"), 2000}
                }}, 
                object::pair{std::string("text"), std::string("Current state information")}, 
                object::pair{std::string("recentMessages"), std::string("Some recent messages")}, 
                object::pair{std::string("recentMessagesData"), emptyMemoryData}, 
                object::pair{std::string("bio"), std::string("Agent bio")}, 
                object::pair{std::string("lore"), std::string("Agent lore")}, 
                object::pair{std::string("messageDirections"), std::string("Handle messages this way")}, 
                object::pair{std::string("postDirections"), std::string("Handle posts this way")}, 
                object::pair{std::string("roomId"), createTestUUID(456)}, 
                object::pair{std::string("actors"), std::string("User, Agent")}
            };
            auto stateV2 = toV2State(stateV1);
            expect(stateV2->values)->toBeDefined();
            expect(stateV2->data)->toBeDefined();
            expect(stateV2->text)->toBe(std::string("Current state information"));
            expect(stateV2->userId)->toBe(createTestUUID(123));
            expect(stateV2->agentName)->toBe(std::string("TestAgent"));
            expect(stateV2->bio)->toBe(std::string("Agent bio"));
            expect(stateV2->lore)->toBe(std::string("Agent lore"));
            expect(stateV2->recentMessagesData)->toEqual(emptyMemoryData);
        }
        );
        it(std::string("should handle empty or undefined values"), [=]() mutable
        {
            auto emptyV2 = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            auto emptyV1 = fromV2State(emptyV2);
            auto backToV2 = toV2State(emptyV1);
            expect(emptyV1)->toEqual(object{
                object::pair{std::string("bio"), string_empty}, 
                object::pair{std::string("lore"), string_empty}, 
                object::pair{std::string("messageDirections"), string_empty}, 
                object::pair{std::string("postDirections"), string_empty}, 
                object::pair{std::string("actors"), string_empty}, 
                object::pair{std::string("recentMessages"), string_empty}, 
                object::pair{std::string("recentMessagesData"), array<any>()}, 
                object::pair{std::string("text"), string_empty}
            });
            expect(backToV2)->toEqual(object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}, 
                object::pair{std::string("bio"), string_empty}, 
                object::pair{std::string("lore"), string_empty}, 
                object::pair{std::string("messageDirections"), string_empty}, 
                object::pair{std::string("postDirections"), string_empty}, 
                object::pair{std::string("actors"), string_empty}, 
                object::pair{std::string("recentMessages"), string_empty}, 
                object::pair{std::string("recentMessagesData"), array<any>()}
            });
        }
        );
        it(std::string("should handle additional properties from real-world plugins"), [=]() mutable
        {
            auto tonStateV1 = object{
                object::pair{std::string("userId"), createTestUUID(123)}, 
                object::pair{std::string("agentName"), std::string("TonBot")}, 
                object::pair{std::string("walletAddress"), std::string("0x123abc")}, 
                object::pair{std::string("walletBalance"), 10.5}, 
                object::pair{std::string("stakedAmount"), 5.25}, 
                object::pair{std::string("lastTransaction"), std::string("2023-04-01")}, 
                object::pair{std::string("roomId"), createTestUUID(456)}, 
                object::pair{std::string("recentMessages"), std::string("Recent messages here")}, 
                object::pair{std::string("recentMessagesData"), emptyMemoryData}, 
                object::pair{std::string("bio"), std::string("TON blockchain assistant")}, 
                object::pair{std::string("lore"), std::string("Helps with TON transactions")}, 
                object::pair{std::string("messageDirections"), std::string("Handle DMs from users")}, 
                object::pair{std::string("postDirections"), std::string("Post updates about TON")}, 
                object::pair{std::string("actors"), std::string("User, TonBot")}, 
                object::pair{std::string("text"), std::string("Current state")}
            };
            auto tonStateV2 = toV2State(tonStateV1);
            auto tonStateV1Again = fromV2State(tonStateV2);
            expect(tonStateV1Again->walletAddress)->toBe(std::string("0x123abc"));
            expect(tonStateV1Again->walletBalance)->toBe(10.5);
            expect(tonStateV1Again->stakedAmount)->toBe(5.25);
            expect(tonStateV1Again->lastTransaction)->toBe(std::string("2023-04-01"));
            expect(tonStateV1Again->recentMessagesData)->toEqual(emptyMemoryData);
        }
        );
    }
    );
}

MAIN
