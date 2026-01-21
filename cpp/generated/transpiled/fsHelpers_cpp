#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/fsHelpers.h"

void ensureDir(string dirPath)
{
    std::async([=]() { fs->mkdir(dirPath, object{
        object::pair{std::string("recursive"), true}
    }); });
};


void writeToFile(string filePath, string data)
{
    std::async([=]() { ensureDir(path->dirname(filePath)); });
    std::async([=]() { fs->writeFile(filePath, data, std::string("utf-8")); });
};


any getRepoFilePath(string outputDir, string repoId, string dataType, string intervalType, string fileName)
{
    auto safeRepoId = repoId->replace(std::string("/"), std::string("_"));
    return path->join(outputDir, safeRepoId, dataType, intervalType, fileName);
};


