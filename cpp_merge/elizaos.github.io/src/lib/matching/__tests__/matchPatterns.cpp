#include "matchPatterns.test.h"

void Main(void)
{
    describe(std:("Pattern Matching Functions"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            clearMatcherCache();
        }
        );
        shared patterns = array<std::shared_ptr<TagPattern>>{ object{
            object::pair{std:("pattern"), std:("typescript")}, 
            object::pair{std:("patternType"), PatternType::STRING}, 
            object::pair{std:("target"), TagPatternType::COMMIT_MESSAGE}, 
            object::pair{std:("options"), object{}}
        }, object{
            object::pair{std:("pattern"), std:("feature")}, 
            object::pair{std:("patternType"), PatternType::STRING}, 
            object::pair{std:("target"), TagPatternType::COMMIT_MESSAGE}, 
            object::pair{std:("options"), object{}}
        }, object{
            object::pair{std:("pattern"), std:("bug")}, 
            object::pair{std:("patternType"), PatternType::STRING}, 
            object::pair{std:("target"), TagPatternType::COMMIT_MESSAGE}, 
            object::pair{std:("options"), object{}}
        }, object{
            object::pair{std:("pattern"), std:("*.ts")}, 
            object::pair{std:("patternType"), PatternType::GLOB}, 
            object::pair{std:("target"), TagPatternType::FILE_PATH}, 
            object::pair{std:("options"), object{}}
        }, object{
            object::pair{std:("pattern"), std:("experimental")}, 
            object::pair{std:("patternType"), PatternType::STRING}, 
            object::pair{std:("target"), TagPatternType::COMMIT_MESSAGE}, 
            object::pair{std:("options"), object{
                object::pair{std:("negative"), true}
            }}
        }, object{
            object::pair{std:("pattern"), std:("TODO")}, 
            object::pair{std:("patternType"), PatternType::STRING}, 
            object::pair{std:("target"), TagPatternType::CODE_CONTENT}, 
            object::pair{std:("options"), object{
                object::pair{std:("caseSensitive"), true}
            }}
        }, object{
            object::pair{std:("pattern"), std:("refactor")}, 
            object::pair{std:("patternType"), PatternType::STRING}, 
            object::pair{std:("target"), TagPatternType::COMMIT_MESSAGE}, 
            object::pair{std:("options"), object{
                object::pair{std:("wordBoundary"), true}
            }}
        } };
        describe(std:("matchPatterns"), [=]() mutable
        {
            it(std:("should return patterns matching the content"), [=]() mutable
            {
                auto content = object{
                    object::pair{std:("content"), std:("feat: Add amazing new typescript feature")}, 
                    object::pair{std:("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto matches = matchPatterns(patterns, content);
                expect(matches)->toEqual(array<std::shared_ptr<TagPattern>>{ const_(patterns)[0], const_(patterns)[1] });
            }
            );
            it(std:("should return patterns matching the content respecting word boundary"), [=]() mutable
            {
                auto content = object{
                    object::pair{std:("content"), std:("refactor: improve logic")}, 
                    object::pair{std:("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto content2 = object{
                    object::pair{std:("content"), std:("refactoring: improve logic")}, 
                    object::pair{std:("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto matches = matchPatterns(patterns, content);
                auto matches2 = matchPatterns(patterns, content2);
                expect(matches)->toEqual(array<std::shared_ptr<TagPattern>>{ const_(patterns)[6] });
                expect(matches2)->toEqual(array<any>());
            }
            );
            it(std:("should return empty array if no patterns match"), [=]() mutable
            {
                auto content = object{
                    object::pair{std:("content"), std:("fix: resolve minor issue")}, 
                    object::pair{std:("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto matches = matchPatterns(patterns, content);
                expect(matches)->toEqual(array<any>());
            }
            );
            it(std:("should only match patterns for the correct content type"), [=]() mutable
            {
                auto contentCommit = object{
                    object::pair{std:("content"), std:("feat: Add amazing new typescript feature")}, 
                    object::pair{std:("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto contentFile = object{
                    object::pair{std:("content"), std:("component.ts")}, 
                    object::pair{std:("contentType"), TagPatternType::FILE_PATH}
                };
                auto contentCode = object{
                    object::pair{std:("content"), std:("// TODO: fix this later")}, 
                    object::pair{std:("contentType"), TagPatternType::CODE_CONTENT}
                };
                expect(matchPatterns(patterns, contentCommit))->toEqual(array<std::shared_ptr<TagPattern>>{ const_(patterns)[0], const_(patterns)[1] });
                expect(matchPatterns(patterns, contentFile))->toEqual(array<std::shared_ptr<TagPattern>>{ const_(patterns)[3] });
                expect(matchPatterns(patterns, contentCode))->toEqual(array<std::shared_ptr<TagPattern>>{ const_(patterns)[5] });
            }
            );
            it(std:("should handle case sensitivity correctly"), [=]() mutable
            {
                auto contentCodeLower = object{
                    object::pair{std:("content"), std:("// todo: fix this later")}, 
                    object::pair{std:("contentType"), TagPatternType::CODE_CONTENT}
                };
                auto contentCodeUpper = object{
                    object::pair{std:("content"), std:("// TODO: fix this later")}, 
                    object::pair{std:("contentType"), TagPatternType::CODE_CONTENT}
                };
                expect(matchPatterns(patterns, contentCodeLower))->toEqual(array<any>());
                expect(matchPatterns(patterns, contentCodeUpper))->toEqual(array<std::shared_ptr<TagPattern>>{ const_(patterns)[5] });
            }
            );
            it(std:("should handle negative patterns correctly"), [=]() mutable
            {
                auto contentWithoutNegative = object{
                    object::pair{std:("content"), std:("feat: Add typescript feature")}, 
                    object::pair{std:("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto contentWithNegative = object{
                    object::pair{std:("content"), std:("feat: Add experimental typescript feature")}, 
                    object::pair{std:("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                expect(matchPatterns(patterns, contentWithoutNegative))->toEqual(array<std::shared_ptr<TagPattern>>{ const_(patterns)[0], const_(patterns)[1] });
                auto matcherFn0 = getMatcherFunction(const_(patterns)[0]);
                auto matcherFn1 = getMatcherFunction(const_(patterns)[1]);
                auto matcherFn4 = getMatcherFunction(const_(patterns)[4]);
                expect(matcherFn0(contentWithoutNegative))->toBe(true);
                expect(matcherFn1(contentWithoutNegative))->toBe(true);
                expect(matcherFn4(contentWithoutNegative))->toBe(true);
                expect(matcherFn0(contentWithNegative))->toBe(true);
                expect(matcherFn1(contentWithNegative))->toBe(true);
                expect(matcherFn4(contentWithNegative))->toBe(false);
                expect(matchPatterns(patterns, contentWithoutNegative))->toEqual(array<std::shared_ptr<TagPattern>>{ const_(patterns)[0], const_(patterns)[1] });
                expect(matchPatterns(patterns, contentWithNegative))->toEqual(array<any>());
            }
            );
            it(std:("should return empty array for empty patterns list"), [=]() mutable
            {
                auto content = object{
                    object::pair{std:("content"), std:("feat: Add amazing new typescript feature")}, 
                    object::pair{std:("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                expect(matchPatterns(array<any>(), content))->toEqual(array<any>());
            }
            );
            it(std:("should return empty array for empty content string"), [=]() mutable
            {
                auto content = object{
                    object::pair{std:("content"), string_empty}, 
                    object::pair{std:("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                expect(matchPatterns(patterns, content))->toEqual(array<any>());
            }
            );
        }
        );
        describe(std:("matchAnyPattern"), [=]() mutable
        {
            it(std:("should return true if any pattern matches"), [=]() mutable
            {
                auto contentCommit = object{
                    object::pair{std:("content"), std:("feat: Add amazing new typescript feature")}, 
                    object::pair{std:("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto contentFile = object{
                    object::pair{std:("content"), std:("src/component.ts")}, 
                    object::pair{std:("contentType"), TagPatternType::FILE_PATH}
                };
                auto rootContentFile = object{
                    object::pair{std:("content"), std:("component.ts")}, 
                    object::pair{std:("contentType"), TagPatternType::FILE_PATH}
                };
                auto contentCode = object{
                    object::pair{std:("content"), std:("// TODO: fix this later")}, 
                    object::pair{std:("contentType"), TagPatternType::CODE_CONTENT}
                };
                expect(matchAnyPattern(patterns, contentCommit))->toBe(true);
                expect(matchAnyPattern(patterns, contentFile))->toBe(false);
                expect(matchAnyPattern(patterns, rootContentFile))->toBe(true);
                expect(matchAnyPattern(patterns, contentCode))->toBe(true);
            }
            );
            it(std:("should return false if no pattern matches"), [=]() mutable
            {
                auto content = object{
                    object::pair{std:("content"), std:("docs: experimental update README")}, 
                    object::pair{std:("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                expect(matchAnyPattern(patterns, content))->toBe(false);
            }
            );
            it(std:("should return false if patterns list is empty"), [=]() mutable
            {
                auto content = object{
                    object::pair{std:("content"), std:("feat: Add amazing new typescript feature")}, 
                    object::pair{std:("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                expect(matchAnyPattern(array<any>(), content))->toBe(false);
            }
            );
            it(std:("should return false if content string is empty"), [=]() mutable
            {
                auto content = object{
                    object::pair{std:("content"), string_empty}, 
                    object::pair{std:("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                expect(matchAnyPattern(patterns, content))->toBe(false);
            }
            );
            it(std:("should respect negative patterns"), [=]() mutable
            {
                auto contentWithoutNegative = object{
                    object::pair{std:("content"), std:("feat: Add typescript feature")}, 
                    object::pair{std:("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto contentWithNegative = object{
                    object::pair{std:("content"), std:("feat: Add experimental typescript feature")}, 
                    object::pair{std:("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto contentOnlyNegative = object{
                    object::pair{std:("content"), std:("experimental feature")}, 
                    object::pair{std:("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto contentNoMatches = object{
                    object::pair{std:("content"), std:("just experimental")}, 
                    object::pair{std:("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                expect(matchAnyPattern(patterns, contentWithoutNegative))->toBe(true);
                expect(matchAnyPattern(patterns, contentWithNegative))->toBe(true);
                expect(matchAnyPattern(patterns, contentOnlyNegative))->toBe(true);
                expect(matchAnyPattern(patterns, contentNoMatches))->toBe(false);
            }
            );
        }
        );
    }
    );
}

MAIN
