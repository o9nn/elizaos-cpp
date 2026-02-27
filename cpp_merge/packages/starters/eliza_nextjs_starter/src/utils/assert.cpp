#include "assert.hpp"

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


