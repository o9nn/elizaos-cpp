#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/autodoc/src/DirectoryTraversal.h"

array<string> DirectoryTraversal::FORCED_EXCLUDED_DIRS = array<string>{ std:("node_modules"), std:(".git"), std:("dist"), std:("build"), std:("coverage"), std:(".next"), std:(".nuxt"), std:(".cache"), std:("tmp"), std:("temp"), std:(".turbo"), std:(".husky"), std:(".github"), std:(".vscode"), std:("public"), std:("static") };

DirectoryTraversal::DirectoryTraversal(std::shared_ptr<Configuration> config_, array<string> prFiles_) : config(config_), prFiles(prFiles_)  {
}

string DirectoryTraversal::getAbsolutePath(string filePath)
{
    return this->config->toAbsolutePath(filePath);
}

string DirectoryTraversal::getRelativePath(string filePath)
{
    return this->config->toRelativePath(filePath);
}

array<string> DirectoryTraversal::traverse()
{
    if (this->prFiles->get_length() > 0) {
        console->log(std:("Detected PR Files:"), this->prFiles);
        auto files = this->prFiles->filter([=](auto file) mutable
        {
            auto absolutePath = this->config->toAbsolutePath(file);
            auto isInTargetDir = absolutePath->startsWith(this->config->get_absolutePath());
            return (AND((AND((AND((isInTargetDir), (fs->existsSync(absolutePath)))), (!this->isExcluded(absolutePath)))), (path->extname(file) == std:(".ts"))));
        }
        )->map([=](auto file) mutable
        {
            return this->config->toAbsolutePath(file);
        }
        );
        console->log(std:("Files to process:"), files);
        return files;
    }
    console->log(std:("No PR Files Detected, Scanning all files in root directory"));
    shared typeScriptFiles = array<string>();
    shared traverseDirectory = [=](auto currentDirectory) mutable
    {
        auto files = fs->readdirSync(currentDirectory);
        files->forEach([=](auto file) mutable
        {
            auto filePath = path->join(currentDirectory, file);
            auto stats = fs->statSync(filePath);
            if (stats->isDirectory()) {
                if (!this->isExcluded(filePath)) {
                    traverseDirectory(filePath);
                }
            } else if (AND((stats->isFile()), (!this->isExcluded(filePath)))) {
                if (path->extname(file) == std:(".ts")) {
                    typeScriptFiles->push(filePath);
                }
            }
        }
        );
    };
    traverseDirectory(this->config->get_absolutePath());
    return typeScriptFiles;
}

boolean DirectoryTraversal::isExcluded(string absolutePath)
{
    shared relativeToTarget = path->relative(this->config->get_absolutePath(), absolutePath);
    auto isInForcedExcludedDir = DirectoryTraversal::FORCED_EXCLUDED_DIRS->some([=](auto dir) mutable
    {
        return OR((OR((absolutePath->includes(string_empty + path->sep + string_empty + dir + string_empty + path->sep + string_empty)), (absolutePath->includes(string_empty + path->sep + string_empty + dir + string_empty)))), (absolutePath->startsWith(string_empty + dir + string_empty + path->sep + string_empty)));
    }
    );
    if (isInForcedExcludedDir) {
        return true;
    }
    auto isExcludedDir = this->config->excludedDirectories->some([=](auto dir) mutable
    {
        return const_(relativeToTarget->split(path->sep))[0] == dir;
    }
    );
    auto isExcludedFile = this->config->excludedFiles->some([=](auto file) mutable
    {
        return path->basename(absolutePath) == file;
    }
    );
    return OR((isExcludedDir), (isExcludedFile));
}

