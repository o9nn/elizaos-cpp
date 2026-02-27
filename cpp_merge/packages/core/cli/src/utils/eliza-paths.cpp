#include "eliza-paths.h"
#include <string>

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


