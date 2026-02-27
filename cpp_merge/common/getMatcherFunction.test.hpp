#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_MATCHING___TESTS___GETMATCHERFUNCTION_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_MATCHING___TESTS___GETMATCHERFUNCTION_TEST_H
#include "core.h"
#include "bun:test.h"
#include "../getMatcherFunction.h"
#include "../types.h"
#include "../../data/tags.h"

extern double createMatcherCallCount;
extern boolean shouldThrowError;
extern std::function<std::shared_ptr<MatcherFunction>(std::shared_ptr<TagPattern>)> originalCreateMatcher;
extern std::function<std::shared_ptr<MatcherFunction>(std::shared_ptr<TagPattern>)> mockCreateMatcher;
#endif
