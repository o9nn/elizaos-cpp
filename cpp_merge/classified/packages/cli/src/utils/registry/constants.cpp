#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/utils/registry/constants.h"

string REGISTRY_ORG = std::string("elizaos-plugins");
string REGISTRY_REPO_NAME = std::string("registry");
string REGISTRY_REPO = string_empty + REGISTRY_ORG + std::string("/") + REGISTRY_REPO_NAME + string_empty;
string REGISTRY_URL = std::string("https://raw.githubusercontent.com/") + REGISTRY_REPO + std::string("/refs/heads/main/index.json");
string REGISTRY_GITHUB_URL = std::string("https://github.com/") + REGISTRY_REPO + string_empty;
string RAW_REGISTRY_URL = REGISTRY_URL;

void Main(void)
{
}

MAIN
