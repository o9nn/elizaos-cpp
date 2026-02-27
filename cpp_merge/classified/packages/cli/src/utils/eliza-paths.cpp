#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/utils/eliza-paths.h"

string getElizaBaseDir(string cwd)
{
    return path->join(cwd, std:(".eliza"));
};


string getElizaDbDir(string cwd)
{
    return path->join(getElizaBaseDir(cwd), std:(".elizadb"));
};


string getElizaDataDir(string cwd)
{
    return path->join(getElizaBaseDir(cwd), std:("data"));
};


string getElizaUploadsDir(string cwd)
{
    return path->join(getElizaDataDir(cwd), std:("uploads"));
};


string getElizaGeneratedDir(string cwd)
{
    return path->join(getElizaDataDir(cwd), std:("generated"));
};


string getElizaCharactersDir(string cwd)
{
    return path->join(getElizaDataDir(cwd), std:("characters"));
};


