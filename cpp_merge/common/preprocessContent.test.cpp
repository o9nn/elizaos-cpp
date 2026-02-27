#include "preprocessContent.test.h"

void Main(void)
{
    describe(std:("Content Preprocessing & Validation"), [=]() mutable
    {
        describe(std:("preprocessContent"), [=]() mutable
        {
            it(std:("should normalize file paths"), [=]() mutable
            {
                auto content = object{
                    object::pair{std:("content"), std:("src\lib\matching\file.ts ")}, 
                    object::pair{std:("contentType"), TagPatternType::FILE_PATH}
                };
                auto processed = preprocessContent(content);
                expect(processed["content"])->toBe(std:("src/lib/matching/file.ts"));
            }
            );
            it(std:("should trim text content"), [=]() mutable
            {
                auto content = object{
                    object::pair{std:("content"), std:("  Some commit message  ")}, 
                    object::pair{std:("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto processed = preprocessContent(content);
                expect(processed["content"])->toBe(std:("Some commit message"));
            }
            );
            it(std:("should normalize line endings in code content"), [=]() mutable
            {
                auto content = object{
                    object::pair{std:("content"), std:("function test() {
\
  return true;
\
}")}, 
                    object::pair{std:("contentType"), TagPatternType::CODE_CONTENT}
                };
                auto processed = preprocessContent(content);
                expect(processed["content"])->toBe(std:("function test() {\
  return true;\
}"));
            }
            );
            it(std:("should trim and lowercase label content"), [=]() mutable
            {
                auto content = object{
                    object::pair{std:("content"), std:("  FEATURE  ")}, 
                    object::pair{std:("contentType"), TagPatternType::LABEL}
                };
                auto processed = preprocessContent(content);
                expect(processed["content"])->toBe(std:("feature"));
            }
            );
            it(std:("should not modify content for types without specific preprocessing"), [=]() mutable
            {
                auto content = object{
                    object::pair{std:("content"), std:("  Reaction Content ")}, 
                    object::pair{std:("contentType"), TagPatternType::REACTION}
                };
                auto processed = preprocessContent(content);
                expect(processed["content"])->toBe(std:("  Reaction Content "));
            }
            );
        }
        );
        describe(std:("validatePatternForTarget"), [=]() mutable
        {
            it(std:("should validate GLOB only for FILE_PATH"), [=]() mutable
            {
                expect(validatePatternForTarget(PatternType::GLOB, TagPatternType::FILE_PATH))->toBe(true);
                expect(validatePatternForTarget(PatternType::GLOB, TagPatternType::COMMIT_MESSAGE))->toBe(false);
                expect(validatePatternForTarget(PatternType::GLOB, TagPatternType::CODE_CONTENT))->toBe(false);
            }
            );
            it(std:("should validate STRING for text-based targets"), [=]() mutable
            {
                expect(validatePatternForTarget(PatternType::STRING, TagPatternType::COMMIT_MESSAGE))->toBe(true);
                expect(validatePatternForTarget(PatternType::STRING, TagPatternType::FILE_PATH))->toBe(true);
                expect(validatePatternForTarget(PatternType::STRING, TagPatternType::LABEL))->toBe(true);
            }
            );
            it(std:("should validate REGEX for text-based targets"), [=]() mutable
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
