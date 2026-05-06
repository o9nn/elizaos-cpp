#include "utils.hpp"
#include <string>

string expandTildePath(string filepath)
{
    if (OR((!filepath), (type_of(filepath) != std::string("string")))) {
        return filepath;
    }
    if (filepath->startsWith(std::string("~"))) {
        return path->join(process->cwd(), filepath->slice(1));
    }
    return filepath;
};


string resolvePgliteDir(string dir, string fallbackDir)
{
    if (AND((dir), (dir->trim()))) {
        return expandTildePath(dir);
    }
    auto envFile = std::string(".env");
    if (fs->existsSync(envFile)) {
        dotenv->config(object{
            object::pair{std::string("path"), envFile}
        });
    }
    auto envDir = process->env->PGLITE_DIR;
    if (envDir) {
        return expandTildePath(envDir);
    }
    if (fallbackDir) {
        return expandTildePath(fallbackDir);
    }
    return expandTildePath(std::string("~/eliza/data"));
};


