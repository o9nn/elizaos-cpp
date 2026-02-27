#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SCRIPTS_GENERATE_UNIT_TESTS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SCRIPTS_GENERATE_UNIT_TESTS_H
#include "core.hpp"
#include "fs/promises.h"
#include "path.hpp"
#include "fs.hpp"

class FileToTest;

class FileToTest : public object, public std::enable_shared_from_this<FileToTest> {
public:
    using std::enable_shared_from_this<FileToTest>::shared_from_this;
    string sourcePath;

    string testPath;

    any category;
};

std::shared_ptr<Promise<array<string>>> findAllSourceFiles(string dir, array<string> files = array<string>());

template <typename RET>
RET categorizeFile(string filePath);

string getTestPath(string sourcePath, string category);

std::shared_ptr<Promise<string>> generateTestContent(string sourcePath);

void main();

template <typename RET>
RET categorizeFile(string filePath)
{
    if (filePath->includes(std:("/commands/"))) return std:("commands");
    if (filePath->includes(std:("/utils/"))) return std:("utils");
    if (filePath->includes(std:("/types/"))) return std:("types");
    return std:("other");
};


#endif
