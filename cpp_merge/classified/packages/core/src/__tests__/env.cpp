#include "env.test.h"

void Main(void)
{
    describe(std:("Environment Setup"), [=]() mutable
    {
        it(std:("should verify .env.test file exists"), [=]() mutable
        {
            auto possiblePaths = array<any>{ path->join(process->cwd(), std:(".env.test")), path->join(process->cwd(), std:("packages/core/.env.test")), path->join(__dirname, std:("../../.env.test")), path->join(__dirname, std:("../.env.test")), path->join(__dirname, std:(".env.test")) };
            auto existingPaths = possiblePaths->filter([=](auto p) mutable
            {
                auto exists = fs->existsSync(p);
                console->log(std:("Path ") + p + std:(" exists: ") + exists + string_empty);
                return exists;
            }
            );
            expect(existingPaths->get_length())->toBeGreaterThan(0);
        }
        );
    }
    );
}

MAIN
