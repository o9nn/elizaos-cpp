#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAOS_GITHUB_IO_SRC_LIB_MATCHING_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAOS_GITHUB_IO_SRC_LIB_MATCHING_TYPES_H
#include "core.hpp"
#include <string>
#include "../data/tags.h"

typedef object MatchContent;

enum struct PatternType;
class PatternOptions;
class TagPattern;

enum struct PatternType {
    STRING = std::string("STRING"), REGEX = std::string("REGEX"), GLOB = std::string("GLOB")
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
