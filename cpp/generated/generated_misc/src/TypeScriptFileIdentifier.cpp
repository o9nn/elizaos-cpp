#include "auto.fun/packages/autodoc/src/TypeScriptFileIdentifier.h"

boolean TypeScriptFileIdentifier::isTypeScriptFile(std::string file)
{
    auto extension = path->extname(file);
    return OR((extension == std::string(".ts")), (extension == std::string(".tsx")));
}

array<string> TypeScriptFileIdentifier::getTypeScriptFiles(std::string directory)
{
    auto files = fs->readdirSync(directory);
    return files->filter([=](auto file) mutable
    {
        return this->isTypeScriptFile(file);
    }
    );
}

