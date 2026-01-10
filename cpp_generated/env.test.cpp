#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v2/__tests__/env.test.h"

void Main(void)
{
    describe(std::string("Environment Setup"), [=]() mutable
    {
        it(std::string("should verify .env.test file exists"), [=]() mutable
        {
            auto possiblePaths = array<any>{ path->join(process->cwd(), std::string(".env.test")), path->join(process->cwd(), std::string("packages/core/.env.test")), path->join(__dirname, std::string("../../.env.test")), path->join(__dirname, std::string("../.env.test")), path->join(__dirname, std::string(".env.test")) };
            auto existingPaths = possiblePaths->filter([=](auto p) mutable
            {
                auto exists = fs->existsSync(p);
                console->log(std::string("Path ") + p + std::string(" exists: ") + exists + string_empty);
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
