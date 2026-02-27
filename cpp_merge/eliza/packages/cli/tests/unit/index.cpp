#include "index.test.h"
#include <string>
#include <vector>

void Main(void)
{
    describe(std:("CLI argument parsing logic"), [=]() mutable
    {
        it(std:("should detect --no-emoji flag in std::vector<std::string>()"), [=]() mutable
        {
            auto testArgv = array<string>{ std:("node"), std:("elizaos"), std:("--no-emoji") };
            auto hasNoEmojiFlag = testArgv->includes(std:("--no-emoji"));
            expect(hasNoEmojiFlag)->toBe(true);
        }
        );
        it(std:("should detect --no-auto-install flag in std::vector<std::string>()"), [=]() mutable
        {
            auto testArgv = array<string>{ std:("node"), std:("elizaos"), std:("--no-auto-install") };
            auto hasNoAutoInstallFlag = testArgv->includes(std:("--no-auto-install"));
            expect(hasNoAutoInstallFlag)->toBe(true);
        }
        );
        it(std:("should detect when no arguments are provided"), [=]() mutable
        {
            auto testArgv = array<string>{ std:("node"), std:("elizaos") };
            auto hasNoArgs = testArgv->get_length() == 2;
            expect(hasNoArgs)->toBe(true);
        }
        );
        it(std:("should detect update command"), [=]() mutable
        {
            auto testArgv = array<string>{ std:("node"), std:("elizaos"), std:("update") };
            auto args = testArgv->slice(2);
            auto isUpdateCommand = args->includes(std:("update"));
            expect(isUpdateCommand)->toBe(true);
        }
        );
        it(std:("should detect when banner should be shown"), [=]() mutable
        {
            auto testArgv = array<string>{ std:("node"), std:("elizaos") };
            auto args = testArgv->slice(2);
            auto willShowBanner = args->get_length() == 0;
            expect(willShowBanner)->toBe(true);
        }
        );
        it(std:("should not show banner when command is provided"), [=]() mutable
        {
            auto testArgv = array<string>{ std:("node"), std:("elizaos"), std:("start") };
            auto args = testArgv->slice(2);
            auto willShowBanner = args->get_length() == 0;
            expect(willShowBanner)->toBe(false);
        }
        );
    }
    );
}

MAIN
