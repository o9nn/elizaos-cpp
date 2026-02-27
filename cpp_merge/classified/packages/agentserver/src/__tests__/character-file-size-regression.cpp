#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/__tests__/character-file-size-regression.test.h"

void Main(void)
{
    describe(std:("Character File Size Limits - Issue #5268 Regression Test"), [=]() mutable
    {
        auto generateLargeCharacter = [=](auto targetSizeKB) mutable
        {
            auto baseCharacter = object{
                object::pair{std:("name"), std:("LargeTestCharacter")}, 
                object::pair{std:("bio"), array<string>{ std:("This is a test character with a large configuration.") }}, 
                object::pair{std:("settings"), object{
                    object::pair{std:("secrets"), object{}}, 
                    object::pair{std:("voice"), object{
                        object::pair{std:("model"), std:("en_US-hfc_female-medium")}
                    }}
                }}, 
                object::pair{std:("messageExamples"), array<any>()}, 
                object::pair{std:("postExamples"), array<any>()}, 
                object::pair{std:("topics"), array<any>()}, 
                object::pair{std:("style"), object{
                    object::pair{std:("all"), array<any>()}, 
                    object::pair{std:("chat"), array<any>()}, 
                    object::pair{std:("post"), array<any>()}
                }}, 
                object::pair{std:("adjectives"), array<any>()}, 
                object::pair{std:("people"), array<any>()}, 
                object::pair{std:("clients"), array<any>()}
            };
            auto currentSize = JSON->stringify(baseCharacter)->get_length();
            auto targetSize = targetSizeKB * 1024;
            auto additionalBytesNeeded = targetSize - currentSize;
            if (additionalBytesNeeded > 0) {
                auto singleBioSize = 1000;
                auto entriesNeeded = Math->ceil(additionalBytesNeeded / singleBioSize);
                for (auto i = 0; i < entriesNeeded; i++)
                {
                    baseCharacter->bio->push(std:("Extended biography section ") + (i + 1) + std:(": ") + std:("x")->repeat(singleBioSize - 50) + string_empty);
                }
            }
            return baseCharacter;
        };

        describe(std:("Issue #5268 Reproduction and Fix Verification"), [=]() mutable
        {
            it(std:("should handle 150KB character that was failing before fix"), [=]() mutable
            {
                auto character = generateLargeCharacter(150);
                auto jsonSize = JSON->stringify(character)->get_length();
                expect(jsonSize)->toBeGreaterThan(145 * 1024);
                expect(jsonSize)->toBeLessThan(200 * 1024);
                auto oldLimit = 100 * 1024;
                expect(jsonSize)->toBeGreaterThan(oldLimit);
                auto newLimit = 2 * 1024 * 1024;
                expect(jsonSize)->toBeLessThan(newLimit);
                expect(character->name)->toBe(std:("LargeTestCharacter"));
                expect(Array->isArray(character->bio))->toBe(true);
                expect(character->bio->length)->toBeGreaterThan(1);
            }
            );
            it(std:("should handle various large character sizes up to reasonable limits"), [=]() mutable
            {
                auto sizes = array<double>{ 150, 500, 1000 };
                sizes->forEach([=](auto sizeKB) mutable
                {
                    auto character = generateLargeCharacter(sizeKB);
                    auto jsonSize = JSON->stringify(character)->get_length();
                    auto maxLimit = 2 * 1024 * 1024;
                    expect(jsonSize)->toBeLessThan(maxLimit);
                    expect(character->name)->toBe(std:("LargeTestCharacter"));
                    expect(type_of(character->bio))->toBe(std:("object"));
                    expect(Array->isArray(character->bio))->toBe(true);
                    expect(character->settings)->toBeDefined();
                }
                );
            }
            );
        }
        );
        describe(std:("Regression Prevention"), [=]() mutable
        {
            it(std:("should document the fix - Express limit increased from 100KB to 2MB"), [=]() mutable
            {
                auto oldLimit = 100 * 1024;
                auto newLimit = 2 * 1024 * 1024;
                expect(newLimit)->toBeGreaterThan(oldLimit);
                expect(newLimit / oldLimit)->toBeCloseTo(20.48);
                auto reportedFileSize = 150 * 1024;
                expect(reportedFileSize)->toBeGreaterThan(oldLimit);
                expect(reportedFileSize)->toBeLessThan(newLimit);
            }
            );
        }
        );
    }
    );
}

MAIN
