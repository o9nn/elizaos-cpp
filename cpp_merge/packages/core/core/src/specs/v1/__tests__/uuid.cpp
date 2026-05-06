#include "uuid.test.h"
#include <string>

void Main(void)
{
    describe(std::string("UUID Module"), [=]() mutable
    {
        describe(std::string("UUID Type"), [=]() mutable
        {
            it(std::string("should define UUID as a string type with specific format"), [=]() mutable
            {
                auto validUUID = std::string("123e4567-e89b-12d3-a456-426614174000");
                expect(validUUID)->toBeDefined();
                expect(type_of(validUUID))->toBe(std::string("string"));
            }
            );
        }
        );
        describe(std::string("asUUID function"), [=]() mutable
        {
            it(std::string("should accept valid UUIDs and normalize to lowercase"), [=]() mutable
            {
                auto validUUIDStrings = array<string>{ std::string("123e4567-e89b-12d3-a456-426614174000"), std::string("a1a2a3a4-b1b2-c1c2-d1d2-d3d4d5d6d7d8"), std::string("00000000-0000-0000-0000-000000000000"), std::string("FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF"), std::string("AbCdEf12-3456-7890-aBcD-eF1234567890") };
                validUUIDStrings->forEach([=](auto validUUID) mutable
                {
                    auto result = asUUID(validUUID);
                    expect(result)->toBe(as<std::shared_ptr<UUID>>(validUUID->toLowerCase()));
                }
                );
            }
            );
            it(std::string("should reject invalid UUIDs"), [=]() mutable
            {
                auto invalidUUIDStrings = array<string>{ string_empty, std::string("123e4567"), std::string("123e4567-e89b-12d3-a456-4266141740001"), std::string("123e4567-e89b-12d3-a456-42661417400g"), std::string("123e4567_e89b_12d3_a456_426614174000"), std::string("123e4567-e89b-12d3-a456-4266-14174000"), std::string("123e4567-e89b-12d3-a456"), nullptr, std::nullopt, std::string("gggggggg-gggg-gggg-gggg-gggggggggggg"), std::string("123456789-1234-1234-1234-123456789012") };
                invalidUUIDStrings->forEach([=](auto invalidUUID) mutable
                {
                    expect([=]() mutable
                    {
                        return asUUID(as<any>(invalidUUID));
                    }
                    )->toThrow(std::string("Invalid UUID format"));
                }
                );
            }
            );
            it(std::string("should convert uppercase UUIDs to lowercase"), [=]() mutable
            {
                auto uppercaseUUID = std::string("ABCDEF12-3456-7890-ABCD-EF1234567890");
                auto expectedLowercase = std::string("abcdef12-3456-7890-abcd-ef1234567890");
                auto result = asUUID(uppercaseUUID);
                expect(result)->toBe(expectedLowercase);
            }
            );
            it(std::string("should return a branded UUID type"), [=]() mutable
            {
                auto uuidStr = std::string("123e4567-e89b-12d3-a456-426614174000");
                auto result = asUUID(uuidStr);
                expect(result)->toBe(uuidStr);
                expect(type_of(result))->toBe(std::string("string"));
            }
            );
            it(std::string("should handle UUIDs with version and variant bits correctly"), [=]() mutable
            {
                auto uuidV4 = std::string("123e4567-e89b-42d3-a456-426614174000");
                auto result = asUUID(uuidV4);
                expect(result)->toBe(uuidV4);
                expect(result->charAt(14))->toBe(std::string("4"));
            }
            );
        }
        );
    }
    );
}

MAIN
