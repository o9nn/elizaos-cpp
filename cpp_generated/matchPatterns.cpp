#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/matching/__tests__/matchPatterns.test.h"

void Main(void)
{
    describe(std::string("Pattern Matching Functions"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            clearMatcherCache();
        }
        );
        shared patterns = array<std::shared_ptr<TagPattern>>{ object{
            object::pair{std::string("pattern"), std::string("typescript")}, 
            object::pair{std::string("patternType"), PatternType::STRING}, 
            object::pair{std::string("target"), TagPatternType::COMMIT_MESSAGE}, 
            object::pair{std::string("options"), object{}}
        }, object{
            object::pair{std::string("pattern"), std::string("feature")}, 
            object::pair{std::string("patternType"), PatternType::STRING}, 
            object::pair{std::string("target"), TagPatternType::COMMIT_MESSAGE}, 
            object::pair{std::string("options"), object{}}
        }, object{
            object::pair{std::string("pattern"), std::string("bug")}, 
            object::pair{std::string("patternType"), PatternType::STRING}, 
            object::pair{std::string("target"), TagPatternType::COMMIT_MESSAGE}, 
            object::pair{std::string("options"), object{}}
        }, object{
            object::pair{std::string("pattern"), std::string("*.ts")}, 
            object::pair{std::string("patternType"), PatternType::GLOB}, 
            object::pair{std::string("target"), TagPatternType::FILE_PATH}, 
            object::pair{std::string("options"), object{}}
        }, object{
            object::pair{std::string("pattern"), std::string("experimental")}, 
            object::pair{std::string("patternType"), PatternType::STRING}, 
            object::pair{std::string("target"), TagPatternType::COMMIT_MESSAGE}, 
            object::pair{std::string("options"), object{
                object::pair{std::string("negative"), true}
            }}
        }, object{
            object::pair{std::string("pattern"), std::string("TODO")}, 
            object::pair{std::string("patternType"), PatternType::STRING}, 
            object::pair{std::string("target"), TagPatternType::CODE_CONTENT}, 
            object::pair{std::string("options"), object{
                object::pair{std::string("caseSensitive"), true}
            }}
        }, object{
            object::pair{std::string("pattern"), std::string("refactor")}, 
            object::pair{std::string("patternType"), PatternType::STRING}, 
            object::pair{std::string("target"), TagPatternType::COMMIT_MESSAGE}, 
            object::pair{std::string("options"), object{
                object::pair{std::string("wordBoundary"), true}
            }}
        } };
        describe(std::string("matchPatterns"), [=]() mutable
        {
            it(std::string("should return patterns matching the content"), [=]() mutable
            {
                auto content = object{
                    object::pair{std::string("content"), std::string("feat: Add amazing new typescript feature")}, 
                    object::pair{std::string("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto matches = matchPatterns(patterns, content);
                expect(matches)->toEqual(array<std::shared_ptr<TagPattern>>{ const_(patterns)[0], const_(patterns)[1] });
            }
            );
            it(std::string("should return patterns matching the content respecting word boundary"), [=]() mutable
            {
                auto content = object{
                    object::pair{std::string("content"), std::string("refactor: improve logic")}, 
                    object::pair{std::string("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto content2 = object{
                    object::pair{std::string("content"), std::string("refactoring: improve logic")}, 
                    object::pair{std::string("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto matches = matchPatterns(patterns, content);
                auto matches2 = matchPatterns(patterns, content2);
                expect(matches)->toEqual(array<std::shared_ptr<TagPattern>>{ const_(patterns)[6] });
                expect(matches2)->toEqual(array<any>());
            }
            );
            it(std::string("should return empty array if no patterns match"), [=]() mutable
            {
                auto content = object{
                    object::pair{std::string("content"), std::string("fix: resolve minor issue")}, 
                    object::pair{std::string("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto matches = matchPatterns(patterns, content);
                expect(matches)->toEqual(array<any>());
            }
            );
            it(std::string("should only match patterns for the correct content type"), [=]() mutable
            {
                auto contentCommit = object{
                    object::pair{std::string("content"), std::string("feat: Add amazing new typescript feature")}, 
                    object::pair{std::string("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto contentFile = object{
                    object::pair{std::string("content"), std::string("component.ts")}, 
                    object::pair{std::string("contentType"), TagPatternType::FILE_PATH}
                };
                auto contentCode = object{
                    object::pair{std::string("content"), std::string("// TODO: fix this later")}, 
                    object::pair{std::string("contentType"), TagPatternType::CODE_CONTENT}
                };
                expect(matchPatterns(patterns, contentCommit))->toEqual(array<std::shared_ptr<TagPattern>>{ const_(patterns)[0], const_(patterns)[1] });
                expect(matchPatterns(patterns, contentFile))->toEqual(array<std::shared_ptr<TagPattern>>{ const_(patterns)[3] });
                expect(matchPatterns(patterns, contentCode))->toEqual(array<std::shared_ptr<TagPattern>>{ const_(patterns)[5] });
            }
            );
            it(std::string("should handle case sensitivity correctly"), [=]() mutable
            {
                auto contentCodeLower = object{
                    object::pair{std::string("content"), std::string("// todo: fix this later")}, 
                    object::pair{std::string("contentType"), TagPatternType::CODE_CONTENT}
                };
                auto contentCodeUpper = object{
                    object::pair{std::string("content"), std::string("// TODO: fix this later")}, 
                    object::pair{std::string("contentType"), TagPatternType::CODE_CONTENT}
                };
                expect(matchPatterns(patterns, contentCodeLower))->toEqual(array<any>());
                expect(matchPatterns(patterns, contentCodeUpper))->toEqual(array<std::shared_ptr<TagPattern>>{ const_(patterns)[5] });
            }
            );
            it(std::string("should handle negative patterns correctly"), [=]() mutable
            {
                auto contentWithoutNegative = object{
                    object::pair{std::string("content"), std::string("feat: Add typescript feature")}, 
                    object::pair{std::string("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto contentWithNegative = object{
                    object::pair{std::string("content"), std::string("feat: Add experimental typescript feature")}, 
                    object::pair{std::string("contentType"), TagPatternType::COMMIT_MESSAGE}
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
            it(std::string("should return empty array for empty patterns list"), [=]() mutable
            {
                auto content = object{
                    object::pair{std::string("content"), std::string("feat: Add amazing new typescript feature")}, 
                    object::pair{std::string("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                expect(matchPatterns(array<any>(), content))->toEqual(array<any>());
            }
            );
            it(std::string("should return empty array for empty content string"), [=]() mutable
            {
                auto content = object{
                    object::pair{std::string("content"), string_empty}, 
                    object::pair{std::string("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                expect(matchPatterns(patterns, content))->toEqual(array<any>());
            }
            );
        }
        );
        describe(std::string("matchAnyPattern"), [=]() mutable
        {
            it(std::string("should return true if any pattern matches"), [=]() mutable
            {
                auto contentCommit = object{
                    object::pair{std::string("content"), std::string("feat: Add amazing new typescript feature")}, 
                    object::pair{std::string("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto contentFile = object{
                    object::pair{std::string("content"), std::string("src/component.ts")}, 
                    object::pair{std::string("contentType"), TagPatternType::FILE_PATH}
                };
                auto rootContentFile = object{
                    object::pair{std::string("content"), std::string("component.ts")}, 
                    object::pair{std::string("contentType"), TagPatternType::FILE_PATH}
                };
                auto contentCode = object{
                    object::pair{std::string("content"), std::string("// TODO: fix this later")}, 
                    object::pair{std::string("contentType"), TagPatternType::CODE_CONTENT}
                };
                expect(matchAnyPattern(patterns, contentCommit))->toBe(true);
                expect(matchAnyPattern(patterns, contentFile))->toBe(false);
                expect(matchAnyPattern(patterns, rootContentFile))->toBe(true);
                expect(matchAnyPattern(patterns, contentCode))->toBe(true);
            }
            );
            it(std::string("should return false if no pattern matches"), [=]() mutable
            {
                auto content = object{
                    object::pair{std::string("content"), std::string("docs: experimental update README")}, 
                    object::pair{std::string("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                expect(matchAnyPattern(patterns, content))->toBe(false);
            }
            );
            it(std::string("should return false if patterns list is empty"), [=]() mutable
            {
                auto content = object{
                    object::pair{std::string("content"), std::string("feat: Add amazing new typescript feature")}, 
                    object::pair{std::string("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                expect(matchAnyPattern(array<any>(), content))->toBe(false);
            }
            );
            it(std::string("should return false if content string is empty"), [=]() mutable
            {
                auto content = object{
                    object::pair{std::string("content"), string_empty}, 
                    object::pair{std::string("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                expect(matchAnyPattern(patterns, content))->toBe(false);
            }
            );
            it(std::string("should respect negative patterns"), [=]() mutable
            {
                auto contentWithoutNegative = object{
                    object::pair{std::string("content"), std::string("feat: Add typescript feature")}, 
                    object::pair{std::string("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto contentWithNegative = object{
                    object::pair{std::string("content"), std::string("feat: Add experimental typescript feature")}, 
                    object::pair{std::string("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto contentOnlyNegative = object{
                    object::pair{std::string("content"), std::string("experimental feature")}, 
                    object::pair{std::string("contentType"), TagPatternType::COMMIT_MESSAGE}
                };
                auto contentNoMatches = object{
                    object::pair{std::string("content"), std::string("just experimental")}, 
                    object::pair{std::string("contentType"), TagPatternType::COMMIT_MESSAGE}
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
