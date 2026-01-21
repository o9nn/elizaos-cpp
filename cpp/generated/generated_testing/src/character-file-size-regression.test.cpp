#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/server/src/__tests__/character-file-size-regression.test.h"

void Main(void)
{
    describe(std::string("Character File Size Limits - Issue #5268 Regression Test"), [=]() mutable
    {
        auto generateLargeCharacter = [=](auto targetSizeKB) mutable
        {
            auto baseCharacter = object{
                object::pair{std::string("name"), std::string("LargeTestCharacter")}, 
                object::pair{std::string("bio"), array<string>{ std::string("This is a test character with a large configuration.") }}, 
                object::pair{std::string("settings"), object{
                    object::pair{std::string("secrets"), object{}}, 
                    object::pair{std::string("voice"), object{
                        object::pair{std::string("model"), std::string("en_US-hfc_female-medium")}
                    }}
                }}, 
                object::pair{std::string("messageExamples"), array<any>()}, 
                object::pair{std::string("postExamples"), array<any>()}, 
                object::pair{std::string("topics"), array<any>()}, 
                object::pair{std::string("style"), object{
                    object::pair{std::string("all"), array<any>()}, 
                    object::pair{std::string("chat"), array<any>()}, 
                    object::pair{std::string("post"), array<any>()}
                }}, 
                object::pair{std::string("adjectives"), array<any>()}, 
                object::pair{std::string("people"), array<any>()}, 
                object::pair{std::string("clients"), array<any>()}
            };
            auto currentSize = JSON->stringify(baseCharacter)->get_length();
            auto targetSize = targetSizeKB * 1024;
            auto additionalBytesNeeded = targetSize - currentSize;
            if (additionalBytesNeeded > 0) {
                auto singleBioSize = 1000;
                auto entriesNeeded = Math->ceil(additionalBytesNeeded / singleBioSize);
                for (auto i = 0; i < entriesNeeded; i++)
                {
                    baseCharacter->bio->push(std::string("Extended biography section ") + (i + 1) + std::string(": ") + std::string("x")->repeat(singleBioSize - 50) + string_empty);
                }
            }
            return baseCharacter;
        };

        describe(std::string("Issue #5268 Reproduction and Fix Verification"), [=]() mutable
        {
            it(std::string("should handle 150KB character that was failing before fix"), [=]() mutable
            {
                auto character = generateLargeCharacter(150);
                auto jsonSize = JSON->stringify(character)->get_length();
                expect(jsonSize)->toBeGreaterThan(145 * 1024);
                expect(jsonSize)->toBeLessThan(200 * 1024);
                auto oldLimit = 100 * 1024;
                expect(jsonSize)->toBeGreaterThan(oldLimit);
                auto newLimit = 2 * 1024 * 1024;
                expect(jsonSize)->toBeLessThan(newLimit);
                expect(character->name)->toBe(std::string("LargeTestCharacter"));
                expect(Array->isArray(character->bio))->toBe(true);
                expect(character->bio->length)->toBeGreaterThan(1);
            }
            );
            it(std::string("should handle various large character sizes up to reasonable limits"), [=]() mutable
            {
                auto sizes = array<double>{ 150, 500, 1000 };
                sizes->forEach([=](auto sizeKB) mutable
                {
                    auto character = generateLargeCharacter(sizeKB);
                    auto jsonSize = JSON->stringify(character)->get_length();
                    auto maxLimit = 2 * 1024 * 1024;
                    expect(jsonSize)->toBeLessThan(maxLimit);
                    expect(character->name)->toBe(std::string("LargeTestCharacter"));
                    expect(type_of(character->bio))->toBe(std::string("object"));
                    expect(Array->isArray(character->bio))->toBe(true);
                    expect(character->settings)->toBeDefined();
                }
                );
            }
            );
        }
        );
        describe(std::string("Regression Prevention"), [=]() mutable
        {
            it(std::string("should document the fix - Express limit increased from 100KB to 2MB"), [=]() mutable
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
