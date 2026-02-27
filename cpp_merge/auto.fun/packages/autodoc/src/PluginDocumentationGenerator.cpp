#include "PluginDocumentationGenerator.hpp"

PluginDocumentationGenerator::PluginDocumentationGenerator(std::shared_ptr<AIService> aiService_, std::shared_ptr<GitManager> gitManager_, std::shared_ptr<Configuration> configuration_) : aiService(aiService_), gitManager(gitManager_), configuration(configuration_)  {
    this->fullDocumentationGenerator = std::make_shared<FullDocumentationGenerator>(configuration);
}

std::shared_ptr<Promise<void>> PluginDocumentationGenerator::generate(array<std::shared_ptr<ASTQueueItem>> existingDocs, string branchName, array<std::shared_ptr<TodoItem>> todoItems, array<std::shared_ptr<EnvUsage>> envUsages)
{
    auto packageJsonPath = path->join(this->configuration->get_absolutePath(), std:("package.json"));
    auto packageJson = JSON->parse(fs->readFileSync(packageJsonPath, std:("utf-8")));
    if (!packageJson) {
        console->error(std:("package.json not found"));
    }
    auto documentation = std::async([=]() { this->fullDocumentationGenerator->generatePluginDocumentation(object{
        object::pair{std:("existingDocs"), std:("existingDocs")}, 
        object::pair{std:("packageJson"), std:("packageJson")}, 
        object::pair{std:("todoItems"), std:("todoItems")}, 
        object::pair{std:("envUsages"), std:("envUsages")}
    }); });
    auto markdownContent = this->generateMarkdownContent(documentation, packageJson);
    if (branchName) {
        auto relativeReadmePath = path->join(this->configuration->get_relativePath(), std:("README-automated.md"));
        std::async([=]() { this->gitManager->commitFile(branchName, relativeReadmePath, markdownContent, std:("docs: Update plugin documentation")); });
    } else {
        console->error(std:("No branch name provided, skipping commit for README-automated.md"));
    }
    return std::shared_ptr<Promise<void>>();
}

string PluginDocumentationGenerator::generateMarkdownContent(std::shared_ptr<PluginDocumentation> docs, any packageJson)
{
    return std:("# ") + packageJson["name"] + std:(" Documentation\
\
## Overview\
") + docs->overview + std:("\
\
## Installation\
") + docs->installation + std:("\
\
## Configuration\
") + docs->configuration + std:("\
\
## Features\
\
### Actions\
") + docs->actionsDocumentation + std:("\
\
### Providers\
") + docs->providersDocumentation + std:("\
\
### Evaluators\
") + docs->evaluatorsDocumentation + std:("\
\
## Usage Examples\
") + docs->usage + std:("\
\
## FAQ\
") + docs->faq + std:("\
\
## Development\
\
### TODO Items\
") + docs->todos + std:("\
\
## Troubleshooting Guide\
") + docs->troubleshooting + string_empty;
}

