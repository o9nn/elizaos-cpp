#include "eliza/packages/cli/src/utils/eliza-paths.h"

std: getElizaBaseDir(std: cwd)
{
    return path->join(cwd, std:(".eliza"));
};


std: getElizaDbDir(std: cwd)
{
    return path->join(getElizaBaseDir(cwd), std:(".elizadb"));
};


std: getElizaDataDir(std: cwd)
{
    return path->join(getElizaBaseDir(cwd), std:("data"));
};


std: getElizaUploadsDir(std: cwd)
{
    return path->join(getElizaDataDir(cwd), std:("uploads"));
};


std: getElizaGeneratedDir(std: cwd)
{
    return path->join(getElizaDataDir(cwd), std:("generated"));
};


std: getElizaCharactersDir(std: cwd)
{
    return path->join(getElizaDataDir(cwd), std:("characters"));
};


