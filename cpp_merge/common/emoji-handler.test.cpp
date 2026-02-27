#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/unit/utils/emoji-handler.test.h"

void Main(void)
{
    mock->module(std::string("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std::string("logger"), object{
                object::pair{std::string("warn"), mock()}, 
                object::pair{std::string("debug"), mock()}
            }}
        });
    }
    );
    describe(std::string("emoji-handler"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            process->env.Delete("NO_COLOR");
            process->env.Delete("CI");
            process->env.Delete("TERM");
            process->env.Delete("TERM_PROGRAM");
            process->env.Delete("COLORTERM");
            process->env.Delete("GITHUB_ACTIONS");
            process->env.Delete("WT_SESSION");
            process->env.Delete("WT_PROFILE_ID");
            process->env.Delete("DEBUG");
            process->env.Delete("ELIZA_DEBUG");
            configureEmojis(object{
                object::pair{std::string("enabled"), true}, 
                object::pair{std::string("forceDisable"), false}
            });
        }
        );
        describe(std::string("getEmoji"), [=]() mutable
        {
            it(std::string("should return emoji when supported"), [=]() mutable
            {
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), std::string("darwin")}, 
                    object::pair{std::string("configurable"), true}
                });
                process->env->TERM = std::string("xterm-256color");
                expect(getEmoji(std::string("success")))->toBe(std::string("✅"));
                expect(getEmoji(std::string("error")))->toBe(std::string("❌"));
                expect(getEmoji(std::string("warning")))->toBe(std::string("⚠️"));
                expect(getEmoji(std::string("info")))->toBe(std::string("ℹ️"));
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), originalPlatform}, 
                    object::pair{std::string("configurable"), true}
                });
            }
            );
            it(std::string("should return fallback when not supported"), [=]() mutable
            {
                configureEmojis(object{
                    object::pair{std::string("forceDisable"), true}
                });
                expect(getEmoji(std::string("success")))->toBe(std::string("[OK]"));
                expect(getEmoji(std::string("error")))->toBe(std::string("[ERROR]"));
                expect(getEmoji(std::string("warning")))->toBe(std::string("[WARNING]"));
                expect(getEmoji(std::string("info")))->toBe(std::string("[INFO]"));
            }
            );
            it(std::string("should return fallback in CI environment"), [=]() mutable
            {
                process->env->CI = std::string("true");
                expect(getEmoji(std::string("success")))->toBe(std::string("[OK]"));
                expect(getEmoji(std::string("error")))->toBe(std::string("[ERROR]"));
            }
            );
            it(std::string("should return fallback on Windows without modern terminal"), [=]() mutable
            {
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), std::string("win32")}, 
                    object::pair{std::string("configurable"), true}
                });
                expect(getEmoji(std::string("success")))->toBe(std::string("[OK]"));
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), originalPlatform}, 
                    object::pair{std::string("configurable"), true}
                });
            }
            );
            it(std::string("should return emoji on Windows with VS Code terminal"), [=]() mutable
            {
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), std::string("win32")}, 
                    object::pair{std::string("configurable"), true}
                });
                process->env->TERM_PROGRAM = std::string("vscode");
                expect(getEmoji(std::string("success")))->toBe(std::string("✅"));
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), originalPlatform}, 
                    object::pair{std::string("configurable"), true}
                });
            }
            );
            it(std::string("should handle unknown emoji key"), [=]() mutable
            {
                expect(getEmoji(std::string("invalid-key")))->toBe(string_empty);
            }
            );
        }
        );
        describe(std::string("configureEmojis and getEmojiConfig"), [=]() mutable
        {
            it(std::string("should update configuration"), [=]() mutable
            {
                configureEmojis(object{
                    object::pair{std::string("enabled"), false}
                });
                auto config = getEmojiConfig();
                expect(config->enabled)->toBe(false);
                expect(config->forceDisable)->toBe(false);
            }
            );
            it(std::string("should merge partial configuration"), [=]() mutable
            {
                configureEmojis(object{
                    object::pair{std::string("forceDisable"), true}
                });
                auto config = getEmojiConfig();
                expect(config->enabled)->toBe(true);
                expect(config->forceDisable)->toBe(true);
            }
            );
        }
        );
        describe(std::string("areEmojisEnabled"), [=]() mutable
        {
            it(std::string("should return true when enabled and supported"), [=]() mutable
            {
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), std::string("darwin")}, 
                    object::pair{std::string("configurable"), true}
                });
                process->env->TERM = std::string("xterm-256color");
                expect(areEmojisEnabled())->toBe(true);
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), originalPlatform}, 
                    object::pair{std::string("configurable"), true}
                });
            }
            );
            it(std::string("should return false when disabled"), [=]() mutable
            {
                configureEmojis(object{
                    object::pair{std::string("enabled"), false}
                });
                expect(areEmojisEnabled())->toBe(false);
            }
            );
            it(std::string("should return false when force disabled"), [=]() mutable
            {
                configureEmojis(object{
                    object::pair{std::string("forceDisable"), true}
                });
                expect(areEmojisEnabled())->toBe(false);
            }
            );
            it(std::string("should return false in CI"), [=]() mutable
            {
                process->env->CI = std::string("true");
                expect(areEmojisEnabled())->toBe(false);
            }
            );
        }
        );
        describe(std::string("withEmoji"), [=]() mutable
        {
            it(std::string("should format message with emoji when supported"), [=]() mutable
            {
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), std::string("darwin")}, 
                    object::pair{std::string("configurable"), true}
                });
                process->env->TERM = std::string("xterm-256color");
                expect(withEmoji(std::string("success"), std::string("Test message")))->toBe(std::string("✅ Test message"));
                expect(withEmoji(std::string("error"), std::string("Error message")))->toBe(std::string("❌ Error message"));
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), originalPlatform}, 
                    object::pair{std::string("configurable"), true}
                });
            }
            );
            it(std::string("should format message with fallback when not supported"), [=]() mutable
            {
                configureEmojis(object{
                    object::pair{std::string("forceDisable"), true}
                });
                expect(withEmoji(std::string("success"), std::string("Test message")))->toBe(std::string("[OK] Test message"));
                expect(withEmoji(std::string("error"), std::string("Error message")))->toBe(std::string("[ERROR] Error message"));
            }
            );
            it(std::string("should handle spacing parameter"), [=]() mutable
            {
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), std::string("darwin")}, 
                    object::pair{std::string("configurable"), true}
                });
                process->env->TERM = std::string("xterm-256color");
                expect(withEmoji(std::string("bullet"), std::string("Item"), false))->toBe(std::string("•Item"));
                expect(withEmoji(std::string("bullet"), std::string("Item"), true))->toBe(std::string("• Item"));
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), originalPlatform}, 
                    object::pair{std::string("configurable"), true}
                });
            }
            );
        }
        );
        describe(std::string("emoji utility functions"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), std::string("darwin")}, 
                    object::pair{std::string("configurable"), true}
                });
                process->env->TERM = std::string("xterm-256color");
            }
            );
            it(std::string("should format success messages"), [=]() mutable
            {
                expect(emoji["success"](std::string("Success!")))->toBe(std::string("✅ Success!"));
            }
            );
            it(std::string("should format error messages"), [=]() mutable
            {
                expect(emoji["error"](std::string("Error!")))->toBe(std::string("❌ Error!"));
            }
            );
            it(std::string("should format warning messages"), [=]() mutable
            {
                expect(emoji["warning"](std::string("Warning!")))->toBe(std::string("⚠️ Warning!"));
            }
            );
            it(std::string("should format info messages"), [=]() mutable
            {
                expect(emoji["info"](std::string("Info!")))->toBe(std::string("ℹ️ Info!"));
            }
            );
            it(std::string("should format rocket messages"), [=]() mutable
            {
                expect(emoji["rocket"](std::string("Launch!")))->toBe(std::string("🚀 Launch!"));
            }
            );
            it(std::string("should format package messages"), [=]() mutable
            {
                expect(emoji["package"](std::string("Package!")))->toBe(std::string("📦 Package!"));
            }
            );
            it(std::string("should format link messages"), [=]() mutable
            {
                expect(emoji["link"](std::string("Link!")))->toBe(std::string("🔗 Link!"));
            }
            );
            it(std::string("should format tip messages"), [=]() mutable
            {
                expect(emoji["tip"](std::string("Tip!")))->toBe(std::string("💡 Tip!"));
            }
            );
            it(std::string("should format list messages"), [=]() mutable
            {
                expect(emoji["list"](std::string("List!")))->toBe(std::string("📋 List!"));
            }
            );
            it(std::string("should format penguin messages"), [=]() mutable
            {
                expect(emoji["penguin"](std::string("Linux!")))->toBe(std::string("🐧 Linux!"));
            }
            );
            it(std::string("should format bullet messages"), [=]() mutable
            {
                expect(emoji["bullet"](std::string("Item")))->toBe(std::string("• Item"));
            }
            );
        }
        );
        describe(std::string("initializeEmojiSupport"), [=]() mutable
        {
            it(std::string("should log emoji support status in debug mode"), [=]() mutable
            {
                process->env->DEBUG = std::string("true");
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), std::string("darwin")}, 
                    object::pair{std::string("configurable"), true}
                });
                process->env->TERM = std::string("xterm-256color");
                initializeEmojiSupport();
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), originalPlatform}, 
                    object::pair{std::string("configurable"), true}
                });
            }
            );
            it(std::string("should not log when not in debug mode"), [=]() mutable
            {
                process->env.Delete("DEBUG");
                process->env.Delete("ELIZA_DEBUG");
                initializeEmojiSupport();
                auto callCount = logger->debug->mock->calls->length;
                expect(callCount)->toBeLessThanOrEqual(1);
            }
            );
        }
        );
        describe(std::string("platform-specific emoji support"), [=]() mutable
        {
            it(std::string("should support emojis on Linux with proper terminal"), [=]() mutable
            {
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), std::string("linux")}, 
                    object::pair{std::string("configurable"), true}
                });
                process->env->TERM = std::string("xterm-256color");
                expect(getEmoji(std::string("success")))->toBe(std::string("✅"));
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), originalPlatform}, 
                    object::pair{std::string("configurable"), true}
                });
            }
            );
            it(std::string("should support emojis with COLORTERM set"), [=]() mutable
            {
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), std::string("linux")}, 
                    object::pair{std::string("configurable"), true}
                });
                process->env->COLORTERM = std::string("truecolor");
                expect(getEmoji(std::string("success")))->toBe(std::string("✅"));
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), originalPlatform}, 
                    object::pair{std::string("configurable"), true}
                });
            }
            );
            it(std::string("should detect Windows Terminal support"), [=]() mutable
            {
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), std::string("win32")}, 
                    object::pair{std::string("configurable"), true}
                });
                process->env->WT_SESSION = std::string("some-session-id");
                expect(getEmoji(std::string("success")))->toBe(std::string("✅"));
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), originalPlatform}, 
                    object::pair{std::string("configurable"), true}
                });
            }
            );
            it(std::string("should detect PowerShell 7+ support"), [=]() mutable
            {
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), std::string("win32")}, 
                    object::pair{std::string("configurable"), true}
                });
                process->env->PSModulePath = std::string("C:\Program Files\PowerShell\7\Modules");
                process->env->POWERSHELL_TELEMETRY_OPTOUT = std::string("1");
                expect(getEmoji(std::string("success")))->toBe(std::string("✅"));
                Object->defineProperty(process, std::string("platform"), object{
                    object::pair{std::string("value"), originalPlatform}, 
                    object::pair{std::string("configurable"), true}
                });
            }
            );
            it(std::string("should not support emojis in GitHub Actions"), [=]() mutable
            {
                process->env->GITHUB_ACTIONS = std::string("true");
                expect(getEmoji(std::string("success")))->toBe(std::string("[OK]"));
            }
            );
        }
        );
    }
    );
}

MAIN
