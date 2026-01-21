#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-nextjs-starter/src/utils/assert.h"

any assert(any condition, string message)
{
    if (!condition) {
        if (process->env->NODE_ENV != std::string("development")) {
            console->error(std::string("Assertion Failed:"), message);
        } else {
            throw any(std::make_shared<Error>(std::string("Assertion Failed: ") + message));
        }
    }
    return any();
};


