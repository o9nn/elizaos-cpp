#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_FORMS_SRC___TESTS___INTEGRATION_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_FORMS_SRC___TESTS___INTEGRATION_TEST_H
#include "core.hpp"
// External dependency removed
#include "uuid.hpp"
using uuidv4 = v4;
#include "bun:test.h"
#include "../index.h"
#include "../providers/forms-provider.h"
#include "../services/forms-service.h"

any createMockRuntime();

extern std::function<any(string)> createMockMemory;
extern std::function<any()> createMockState;
#endif
