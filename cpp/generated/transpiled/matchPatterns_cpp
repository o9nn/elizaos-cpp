#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/matching/matchPatterns.h"

array<std::shared_ptr<TagPattern>> matchPatterns(array<std::shared_ptr<TagPattern>> patterns, MatchContent content)
{
    if (OR((patterns->get_length() == 0), (!content["content"]))) {
        return array<any>();
    }
    auto applicablePatterns = patterns->filter([=](auto pattern) mutable
    {
        return static_cast<long>(pattern->target) == static_cast<long>(content["contentType"]);
    }
    );
    if (applicablePatterns->get_length() == 0) {
        return array<any>();
    }
    auto negativePatterns = applicablePatterns->filter([=](auto p) mutable
    {
        return p->options->negative;
    }
    );
    for (auto& pattern : negativePatterns)
    {
        try
        {
            auto matcherFn = getMatcherFunction(pattern);
            if (!matcherFn(content)) {
                return array<any>();
            }
        }
        catch (const any& error)
        {
            console->error(std::string("Error executing negative matcher for pattern "") + pattern->pattern + std::string("" (type: ") + pattern->patternType + std::string(", target: ") + pattern->target + std::string("):"), error);
        }
    }
    auto matchingPatterns = array<std::shared_ptr<TagPattern>>();
    auto positivePatterns = applicablePatterns->filter([=](auto p) mutable
    {
        return !p->options->negative;
    }
    );
    for (auto& pattern : positivePatterns)
    {
        try
        {
            auto matcherFn = getMatcherFunction(pattern);
            if (matcherFn(content)) {
                matchingPatterns->push(pattern);
            }
        }
        catch (const any& error)
        {
            console->error(std::string("Error executing matcher for pattern "") + pattern->pattern + std::string("" (type: ") + pattern->patternType + std::string(", target: ") + pattern->target + std::string("):"), error);
        }
    }
    return matchingPatterns;
};


boolean matchAnyPattern(array<std::shared_ptr<TagPattern>> patterns, MatchContent content)
{
    if (OR((patterns->get_length() == 0), (!content["content"]))) {
        return false;
    }
    auto applicablePatterns = patterns->filter([=](auto pattern) mutable
    {
        return static_cast<long>(pattern->target) == static_cast<long>(content["contentType"]);
    }
    );
    if (applicablePatterns->get_length() == 0) {
        return false;
    }
    for (auto& pattern : patterns)
    {
        try
        {
            auto matcherFn = getMatcherFunction(pattern);
            if (matcherFn(content)) {
                return true;
            }
        }
        catch (const any& error)
        {
            console->error(std::string("Error executing matcher for pattern "") + pattern->pattern + std::string("" during matchAnyPattern:"), error);
        }
    }
    return false;
};


