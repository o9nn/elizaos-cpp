#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC___TESTS___E2E_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC___TESTS___E2E_INDEX_H
#include "core.h"
#include "@elizaos/core.h"
#include "./basic-functionality.test.h"
using basicFunctionalityTestSuite = _default;
#include "./code-generation.test.h"
using codeGenerationE2ETestSuite = _default;
#include "./claude-code-integration.test.h"
using claudeCodeIntegrationTestSuite = _default;
#include "./claude-code-stress-test.h"
using claudeCodeStressTestSuite = _default;

extern array<std::shared_ptr<TestSuite>> testSuites;
#endif
