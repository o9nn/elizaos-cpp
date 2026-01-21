#include "elizaos.github.io/src/lib/fsHelpers.h"

void ensureDir(std::string dirPath)
{
    std::async([=]() { fs->mkdir(dirPath, object{
        object::pair{std::string("recursive"), true}
    }); });
};


void writeToFile(std::string filePath, std::string data)
{
    std::async([=]() { ensureDir(path->dirname(filePath)); });
    std::async([=]() { fs->writeFile(filePath, data, std::string("utf-8")); });
};


std::any getRepoFilePath(std::string outputDir, std::string repoId, std::string dataType, std::string intervalType, std::string fileName)
{
    auto safeRepoId = repoId->replace(std::string("/"), std::string("_"));
    return path->join(outputDir, safeRepoId, dataType, intervalType, fileName);
};


