#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_TESTS_TEST_FIXTURES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_TESTS_TEST_FIXTURES_H
#include "core.hpp"
#include <string>
#include "fs.hpp"
#include "path.hpp"
#include "os.hpp"

object withTmpEnvFile();

object createTestFileWithContent(string envFile, string content, string fileName = std::string("test.py"));

void makePythonToolImportable(string filePath, string moduleName);

#endif
