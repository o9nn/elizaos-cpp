#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza-nextjs-starter/src/utils/assert.h"

std::any assert(std::any condition, std::string message)
{
    if (!condition) {
        if (process->env->NODE_ENV != std::string("development")) {
            console->error(std::string("Assertion Failed:"), message);
        } else {
            throw std::any(std::make_shared<Error>(std::string("Assertion Failed: ") + message));
        }
    }
    return std::any();
};


