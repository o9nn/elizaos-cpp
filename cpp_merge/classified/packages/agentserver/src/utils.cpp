#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/utils.h"

string expandTildePath(string filepath)
{
    if (OR((!filepath), (type_of(filepath) != std:("string")))) {
        return filepath;
    }
    if (filepath->startsWith(std:("~"))) {
        return path->join(process->cwd(), filepath->slice(1));
    }
    return filepath;
};


string resolvePgliteDir(string dir, string fallbackDir)
{
    if (AND((dir), (dir->trim()))) {
        return expandTildePath(dir);
    }
    auto envFile = std:(".env");
    if (fs->existsSync(envFile)) {
        dotenv->config(object{
            object::pair{std:("path"), envFile}
        });
    }
    auto envDir = process->env->PGLITE_DIR;
    if (envDir) {
        return expandTildePath(envDir);
    }
    if (fallbackDir) {
        return expandTildePath(fallbackDir);
    }
    return expandTildePath(std:("~/eliza/data"));
};


