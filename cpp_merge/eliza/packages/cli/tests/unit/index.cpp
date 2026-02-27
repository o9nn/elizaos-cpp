#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/unit/index.test.h"

void Main(void)
{
    describe(std::string("CLI argument parsing logic"), [=]() mutable
    {
        it(std::string("should detect --no-emoji flag in process.argv"), [=]() mutable
        {
            auto testArgv = array<string>{ std::string("node"), std::string("elizaos"), std::string("--no-emoji") };
            auto hasNoEmojiFlag = testArgv->includes(std::string("--no-emoji"));
            expect(hasNoEmojiFlag)->toBe(true);
        }
        );
        it(std::string("should detect --no-auto-install flag in process.argv"), [=]() mutable
        {
            auto testArgv = array<string>{ std::string("node"), std::string("elizaos"), std::string("--no-auto-install") };
            auto hasNoAutoInstallFlag = testArgv->includes(std::string("--no-auto-install"));
            expect(hasNoAutoInstallFlag)->toBe(true);
        }
        );
        it(std::string("should detect when no arguments are provided"), [=]() mutable
        {
            auto testArgv = array<string>{ std::string("node"), std::string("elizaos") };
            auto hasNoArgs = testArgv->get_length() == 2;
            expect(hasNoArgs)->toBe(true);
        }
        );
        it(std::string("should detect update command"), [=]() mutable
        {
            auto testArgv = array<string>{ std::string("node"), std::string("elizaos"), std::string("update") };
            auto args = testArgv->slice(2);
            auto isUpdateCommand = args->includes(std::string("update"));
            expect(isUpdateCommand)->toBe(true);
        }
        );
        it(std::string("should detect when banner should be shown"), [=]() mutable
        {
            auto testArgv = array<string>{ std::string("node"), std::string("elizaos") };
            auto args = testArgv->slice(2);
            auto willShowBanner = args->get_length() == 0;
            expect(willShowBanner)->toBe(true);
        }
        );
        it(std::string("should not show banner when command is provided"), [=]() mutable
        {
            auto testArgv = array<string>{ std::string("node"), std::string("elizaos"), std::string("start") };
            auto args = testArgv->slice(2);
            auto willShowBanner = args->get_length() == 0;
            expect(willShowBanner)->toBe(false);
        }
        );
    }
    );
}

MAIN
