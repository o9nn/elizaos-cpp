#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_TESTS_TEST-FIXTURES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_TESTS_TEST-FIXTURES_H
#include "core.h"
#include "fs.h"
#include "path.h"
#include "os.h"

object withTmpEnvFile();

object createTestFileWithContent(string envFile, string content, string fileName = std::string("test.py"));

void makePythonToolImportable(string filePath, string moduleName);

#endif
