#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/utils/eliza-paths.h"

string getElizaBaseDir(string cwd)
{
    return path->join(cwd, std::string(".eliza"));
};


string getElizaDbDir(string cwd)
{
    return path->join(getElizaBaseDir(cwd), std::string(".elizadb"));
};


string getElizaDataDir(string cwd)
{
    return path->join(getElizaBaseDir(cwd), std::string("data"));
};


string getElizaUploadsDir(string cwd)
{
    return path->join(getElizaDataDir(cwd), std::string("uploads"));
};


string getElizaGeneratedDir(string cwd)
{
    return path->join(getElizaDataDir(cwd), std::string("generated"));
};


string getElizaCharactersDir(string cwd)
{
    return path->join(getElizaDataDir(cwd), std::string("characters"));
};


