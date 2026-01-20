#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_MATCHING_MATCHPATTERNS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_MATCHING_MATCHPATTERNS_H
#include "core.h"
#include "./types.h"
#include "./getMatcherFunction.h"

array<std::shared_ptr<TagPattern>> matchPatterns(array<std::shared_ptr<TagPattern>> patterns, MatchContent content);

boolean matchAnyPattern(array<std::shared_ptr<TagPattern>> patterns, MatchContent content);

#endif
