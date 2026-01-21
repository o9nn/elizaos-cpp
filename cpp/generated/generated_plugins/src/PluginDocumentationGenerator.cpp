#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/autodoc/src/PluginDocumentationGenerator.h"

PluginDocumentationGenerator::PluginDocumentationGenerator(std::shared_ptr<AIService> aiService_, std::shared_ptr<GitManager> gitManager_, std::shared_ptr<Configuration> configuration_) : aiService(aiService_), gitManager(gitManager_), configuration(configuration_)  {
    this->fullDocumentationGenerator = std::make_shared<FullDocumentationGenerator>(configuration);
}

std::shared_ptr<Promise<void>> PluginDocumentationGenerator::generate(array<std::shared_ptr<ASTQueueItem>> existingDocs, string branchName, array<std::shared_ptr<TodoItem>> todoItems, array<std::shared_ptr<EnvUsage>> envUsages)
{
    auto packageJsonPath = path->join(this->configuration->get_absolutePath(), std::string("package.json"));
    auto packageJson = JSON->parse(fs->readFileSync(packageJsonPath, std::string("utf-8")));
    if (!packageJson) {
        console->error(std::string("package.json not found"));
    }
    auto documentation = std::async([=]() { this->fullDocumentationGenerator->generatePluginDocumentation(object{
        object::pair{std::string("existingDocs"), std::string("existingDocs")}, 
        object::pair{std::string("packageJson"), std::string("packageJson")}, 
        object::pair{std::string("todoItems"), std::string("todoItems")}, 
        object::pair{std::string("envUsages"), std::string("envUsages")}
    }); });
    auto markdownContent = this->generateMarkdownContent(documentation, packageJson);
    if (branchName) {
        auto relativeReadmePath = path->join(this->configuration->get_relativePath(), std::string("README-automated.md"));
        std::async([=]() { this->gitManager->commitFile(branchName, relativeReadmePath, markdownContent, std::string("docs: Update plugin documentation")); });
    } else {
        console->error(std::string("No branch name provided, skipping commit for README-automated.md"));
    }
    return std::shared_ptr<Promise<void>>();
}

string PluginDocumentationGenerator::generateMarkdownContent(std::shared_ptr<PluginDocumentation> docs, any packageJson)
{
    return std::string("# ") + packageJson["name"] + std::string(" Documentation\
\
## Overview\
") + docs->overview + std::string("\
\
## Installation\
") + docs->installation + std::string("\
\
## Configuration\
") + docs->configuration + std::string("\
\
## Features\
\
### Actions\
") + docs->actionsDocumentation + std::string("\
\
### Providers\
") + docs->providersDocumentation + std::string("\
\
### Evaluators\
") + docs->evaluatorsDocumentation + std::string("\
\
## Usage Examples\
") + docs->usage + std::string("\
\
## FAQ\
") + docs->faq + std::string("\
\
## Development\
\
### TODO Items\
") + docs->todos + std::string("\
\
## Troubleshooting Guide\
") + docs->troubleshooting + string_empty;
}

