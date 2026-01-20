#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/matching/__tests__/preprocessContent.test.h"

void Main(void)
{
    describe(std::string("Content Preprocessing & Validation"), [=]() mutable
    {
        describe(std::string("preprocessContent"), [=]() mutable
        {
            it(std::string("should normalize file paths"), [=]() mutable
            {
                auto content = object{
                    object::pair{std::string("content"), std::string("src\lib\matching\file.ts ")}, 
                    object::pair{std::string("contentType"), TagPatternType::FILE_PATH}
                };
                auto processed = preprocessContent(content);
                expect(processed["content"])->toBe(std::string("src/lib/matching/file.ts"));
            }
            );
            it(std::string("should trim text content"), [=]() mutable
            {
                auto content = object{
                    object::pair{std::string("content"), std::string("  Some commit message  ")}, 
                    object::pair{std::string("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto processed = preprocessContent(content);
                expect(processed["content"])->toBe(std::string("Some commit message"));
            }
            );
            it(std::string("should normalize line endings in code content"), [=]() mutable
            {
                auto content = object{
                    object::pair{std::string("content"), std::string("function test() {
\
  return true;
\
}")}, 
                    object::pair{std::string("contentType"), TagPatternType::CODE_CONTENT}
                };
                auto processed = preprocessContent(content);
                expect(processed["content"])->toBe(std::string("function test() {\
  return true;\
}"));
            }
            );
            it(std::string("should trim and lowercase label content"), [=]() mutable
            {
                auto content = object{
                    object::pair{std::string("content"), std::string("  FEATURE  ")}, 
                    object::pair{std::string("contentType"), TagPatternType::LABEL}
                };
                auto processed = preprocessContent(content);
                expect(processed["content"])->toBe(std::string("feature"));
            }
            );
            it(std::string("should not modify content for types without specific preprocessing"), [=]() mutable
            {
                auto content = object{
                    object::pair{std::string("content"), std::string("  Reaction Content ")}, 
                    object::pair{std::string("contentType"), TagPatternType::REACTION}
                };
                auto processed = preprocessContent(content);
                expect(processed["content"])->toBe(std::string("  Reaction Content "));
            }
            );
        }
        );
        describe(std::string("validatePatternForTarget"), [=]() mutable
        {
            it(std::string("should validate GLOB only for FILE_PATH"), [=]() mutable
            {
                expect(validatePatternForTarget(PatternType::GLOB, TagPatternType::FILE_PATH))->toBe(true);
                expect(validatePatternForTarget(PatternType::GLOB, TagPatternType::COMMIT_MESSAGE))->toBe(false);
                expect(validatePatternForTarget(PatternType::GLOB, TagPatternType::CODE_CONTENT))->toBe(false);
            }
            );
            it(std::string("should validate STRING for text-based targets"), [=]() mutable
            {
                expect(validatePatternForTarget(PatternType::STRING, TagPatternType::COMMIT_MESSAGE))->toBe(true);
                expect(validatePatternForTarget(PatternType::STRING, TagPatternType::FILE_PATH))->toBe(true);
                expect(validatePatternForTarget(PatternType::STRING, TagPatternType::LABEL))->toBe(true);
            }
            );
            it(std::string("should validate REGEX for text-based targets"), [=]() mutable
            {
                expect(validatePatternForTarget(PatternType::REGEX, TagPatternType::PR_TITLE))->toBe(true);
                expect(validatePatternForTarget(PatternType::REGEX, TagPatternType::FILE_PATH))->toBe(true);
                expect(validatePatternForTarget(PatternType::REGEX, TagPatternType::CODE_CONTENT))->toBe(true);
            }
            );
        }
        );
    }
    );
}

MAIN
