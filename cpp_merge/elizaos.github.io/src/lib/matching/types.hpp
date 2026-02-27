#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_MATCHING_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_MATCHING_TYPES_H
#include "core.h"
#include "../data/tags.h"

typedef object MatchContent;

enum struct PatternType;
class PatternOptions;
class TagPattern;

enum struct PatternType {
    STRING = std:("STRING"), REGEX = std:("REGEX"), GLOB = std:("GLOB")
};
class PatternOptions : public object, public std::enable_shared_from_this<PatternOptions> {
public:
    using std::enable_shared_from_this<PatternOptions>::shared_from_this;
    boolean caseSensitive;

    boolean wordBoundary;

    boolean negative;
};

class TagPattern : public object, public std::enable_shared_from_this<TagPattern> {
public:
    using std::enable_shared_from_this<TagPattern>::shared_from_this;
    string pattern;

    PatternType patternType;

    TagPatternType target;

    std::shared_ptr<PatternOptions> options;
};

#endif
