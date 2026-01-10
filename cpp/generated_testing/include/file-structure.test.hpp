#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-STARTER_SRC___TESTS___FILE-STRUCTURE_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-STARTER_SRC___TESTS___FILE-STRUCTURE_TEST_H
#include "core.h"
#include "bun:test.h"
#include "node:fs.h"
using fs = _default;
#include "node:path.h"
using path = _default;
#include "@elizaos/core.h"

boolean fileExists(string filePath);

boolean directoryExists(string dirPath);

#endif
