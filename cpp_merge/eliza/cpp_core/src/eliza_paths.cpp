#include "eliza/packages/cli/src/utils/eliza-paths.h"

std::string getElizaBaseDir(std::string cwd)
{
    return path->join(cwd, std::string(".eliza"));
};


std::string getElizaDbDir(std::string cwd)
{
    return path->join(getElizaBaseDir(cwd), std::string(".elizadb"));
};


std::string getElizaDataDir(std::string cwd)
{
    return path->join(getElizaBaseDir(cwd), std::string("data"));
};


std::string getElizaUploadsDir(std::string cwd)
{
    return path->join(getElizaDataDir(cwd), std::string("uploads"));
};


std::string getElizaGeneratedDir(std::string cwd)
{
    return path->join(getElizaDataDir(cwd), std::string("generated"));
};


std::string getElizaCharactersDir(std::string cwd)
{
    return path->join(getElizaDataDir(cwd), std::string("characters"));
};


