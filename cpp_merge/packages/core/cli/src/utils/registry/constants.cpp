#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/utils/registry/constants.h"

string REGISTRY_ORG = std:("elizaos-plugins");
string REGISTRY_REPO_NAME = std:("registry");
string REGISTRY_REPO = string_empty + REGISTRY_ORG + std:("/") + REGISTRY_REPO_NAME + string_empty;
string REGISTRY_URL = std:("https://raw.githubusercontent.com/") + REGISTRY_REPO + std:("/refs/heads/main/index.json");
string REGISTRY_GITHUB_URL = std:("https://github.com/") + REGISTRY_REPO + string_empty;
string RAW_REGISTRY_URL = REGISTRY_URL;

void Main(void)
{
}

MAIN
