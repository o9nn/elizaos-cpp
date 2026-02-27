#include "TypeScriptFileIdentifier.hpp"

boolean TypeScriptFileIdentifier::isTypeScriptFile(string file)
{
    auto extension = path->extname(file);
    return OR((extension == std:(".ts")), (extension == std:(".tsx")));
}

array<string> TypeScriptFileIdentifier::getTypeScriptFiles(string directory)
{
    auto files = fs->readdirSync(directory);
    return files->filter([=](auto file) mutable
    {
        return this->isTypeScriptFile(file);
    }
    );
}

