#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SCRIPTS_GENERATE-UNIT-TESTS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SCRIPTS_GENERATE-UNIT-TESTS_H
#include "core.h"
#include "fs/promises.h"
#include "path.h"
#include "fs.h"

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
    if (filePath->includes(std::string("/commands/"))) return std::string("commands");
    if (filePath->includes(std::string("/utils/"))) return std::string("utils");
    if (filePath->includes(std::string("/types/"))) return std::string("types");
    return std::string("other");
};


#endif
