#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-nextjs-starter/src/utils/assert.h"

any assert(any condition, string message)
{
    if (!condition) {
        if (process->env->NODE_ENV != std:("development")) {
            console->error(std:("Assertion Failed:"), message);
        } else {
            throw any(std::make_shared<Error>(std:("Assertion Failed: ") + message));
        }
    }
    return any();
};


