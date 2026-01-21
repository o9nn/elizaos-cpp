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
    std::string sourcePath;

    std::string testPath;

    std::any category;
};

std::shared_ptr<Promise<array<string>>> findAllSourceFiles(std::string dir, array<string> files = array<string>());

template <typename RET>
RET categorizeFile(std::string filePath);

std::string getTestPath(std::string sourcePath, std::string category);

std::shared_ptr<Promise<string>> generateTestContent(std::string sourcePath);

void main();

template <typename RET>
RET categorizeFile(std::string filePath)
{
    if (filePath->includes(std::string("/commands/"))) return std::string("commands");
    if (filePath->includes(std::string("/utils/"))) return std::string("utils");
    if (filePath->includes(std::string("/types/"))) return std::string("types");
    return std::string("other");
};


#endif
