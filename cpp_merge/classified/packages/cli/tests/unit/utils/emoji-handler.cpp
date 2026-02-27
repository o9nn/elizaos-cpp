#include "emoji-handler.test.h"

void Main(void)
{
    mock->module(std:("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std:("logger"), object{
                object::pair{std:("warn"), mock()}, 
                object::pair{std:("debug"), mock()}
            }}
        });
    }
    );
    describe(std:("emoji-handler"), [=]() mutable
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
                object::pair{std:("enabled"), true}, 
                object::pair{std:("forceDisable"), false}
            });
        }
        );
        describe(std:("getEmoji"), [=]() mutable
        {
            it(std:("should return emoji when supported"), [=]() mutable
            {
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), std:("darwin")}, 
                    object::pair{std:("configurable"), true}
                });
                process->env->TERM = std:("xterm-256color");
                expect(getEmoji(std:("success")))->toBe(std:("✅"));
                expect(getEmoji(std:("error")))->toBe(std:("❌"));
                expect(getEmoji(std:("warning")))->toBe(std:("⚠️"));
                expect(getEmoji(std:("info")))->toBe(std:("ℹ️"));
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), originalPlatform}, 
                    object::pair{std:("configurable"), true}
                });
            }
            );
            it(std:("should return fallback when not supported"), [=]() mutable
            {
                configureEmojis(object{
                    object::pair{std:("forceDisable"), true}
                });
                expect(getEmoji(std:("success")))->toBe(std:("[OK]"));
                expect(getEmoji(std:("error")))->toBe(std:("[ERROR]"));
                expect(getEmoji(std:("warning")))->toBe(std:("[WARNING]"));
                expect(getEmoji(std:("info")))->toBe(std:("[INFO]"));
            }
            );
            it(std:("should return fallback in CI environment"), [=]() mutable
            {
                process->env->CI = std:("true");
                expect(getEmoji(std:("success")))->toBe(std:("[OK]"));
                expect(getEmoji(std:("error")))->toBe(std:("[ERROR]"));
            }
            );
            it(std:("should return fallback on Windows without modern terminal"), [=]() mutable
            {
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), std:("win32")}, 
                    object::pair{std:("configurable"), true}
                });
                expect(getEmoji(std:("success")))->toBe(std:("[OK]"));
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), originalPlatform}, 
                    object::pair{std:("configurable"), true}
                });
            }
            );
            it(std:("should return emoji on Windows with VS Code terminal"), [=]() mutable
            {
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), std:("win32")}, 
                    object::pair{std:("configurable"), true}
                });
                process->env->TERM_PROGRAM = std:("vscode");
                expect(getEmoji(std:("success")))->toBe(std:("✅"));
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), originalPlatform}, 
                    object::pair{std:("configurable"), true}
                });
            }
            );
            it(std:("should handle unknown emoji key"), [=]() mutable
            {
                expect(getEmoji(std:("invalid-key")))->toBe(string_empty);
            }
            );
        }
        );
        describe(std:("configureEmojis and getEmojiConfig"), [=]() mutable
        {
            it(std:("should update configuration"), [=]() mutable
            {
                configureEmojis(object{
                    object::pair{std:("enabled"), false}
                });
                auto config = getEmojiConfig();
                expect(config->enabled)->toBe(false);
                expect(config->forceDisable)->toBe(false);
            }
            );
            it(std:("should merge partial configuration"), [=]() mutable
            {
                configureEmojis(object{
                    object::pair{std:("forceDisable"), true}
                });
                auto config = getEmojiConfig();
                expect(config->enabled)->toBe(true);
                expect(config->forceDisable)->toBe(true);
            }
            );
        }
        );
        describe(std:("areEmojisEnabled"), [=]() mutable
        {
            it(std:("should return true when enabled and supported"), [=]() mutable
            {
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), std:("darwin")}, 
                    object::pair{std:("configurable"), true}
                });
                process->env->TERM = std:("xterm-256color");
                expect(areEmojisEnabled())->toBe(true);
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), originalPlatform}, 
                    object::pair{std:("configurable"), true}
                });
            }
            );
            it(std:("should return false when disabled"), [=]() mutable
            {
                configureEmojis(object{
                    object::pair{std:("enabled"), false}
                });
                expect(areEmojisEnabled())->toBe(false);
            }
            );
            it(std:("should return false when force disabled"), [=]() mutable
            {
                configureEmojis(object{
                    object::pair{std:("forceDisable"), true}
                });
                expect(areEmojisEnabled())->toBe(false);
            }
            );
            it(std:("should return false in CI"), [=]() mutable
            {
                process->env->CI = std:("true");
                expect(areEmojisEnabled())->toBe(false);
            }
            );
        }
        );
        describe(std:("withEmoji"), [=]() mutable
        {
            it(std:("should format message with emoji when supported"), [=]() mutable
            {
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), std:("darwin")}, 
                    object::pair{std:("configurable"), true}
                });
                process->env->TERM = std:("xterm-256color");
                expect(withEmoji(std:("success"), std:("Test message")))->toBe(std:("✅ Test message"));
                expect(withEmoji(std:("error"), std:("Error message")))->toBe(std:("❌ Error message"));
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), originalPlatform}, 
                    object::pair{std:("configurable"), true}
                });
            }
            );
            it(std:("should format message with fallback when not supported"), [=]() mutable
            {
                configureEmojis(object{
                    object::pair{std:("forceDisable"), true}
                });
                expect(withEmoji(std:("success"), std:("Test message")))->toBe(std:("[OK] Test message"));
                expect(withEmoji(std:("error"), std:("Error message")))->toBe(std:("[ERROR] Error message"));
            }
            );
            it(std:("should handle spacing parameter"), [=]() mutable
            {
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), std:("darwin")}, 
                    object::pair{std:("configurable"), true}
                });
                process->env->TERM = std:("xterm-256color");
                expect(withEmoji(std:("bullet"), std:("Item"), false))->toBe(std:("•Item"));
                expect(withEmoji(std:("bullet"), std:("Item"), true))->toBe(std:("• Item"));
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), originalPlatform}, 
                    object::pair{std:("configurable"), true}
                });
            }
            );
        }
        );
        describe(std:("emoji utility functions"), [=]() mutable
        {
            beforeEach([=]() mutable
            {
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), std:("darwin")}, 
                    object::pair{std:("configurable"), true}
                });
                process->env->TERM = std:("xterm-256color");
            }
            );
            it(std:("should format success messages"), [=]() mutable
            {
                expect(emoji["success"](std:("Success!")))->toBe(std:("✅ Success!"));
            }
            );
            it(std:("should format error messages"), [=]() mutable
            {
                expect(emoji["error"](std:("Error!")))->toBe(std:("❌ Error!"));
            }
            );
            it(std:("should format warning messages"), [=]() mutable
            {
                expect(emoji["warning"](std:("Warning!")))->toBe(std:("⚠️ Warning!"));
            }
            );
            it(std:("should format info messages"), [=]() mutable
            {
                expect(emoji["info"](std:("Info!")))->toBe(std:("ℹ️ Info!"));
            }
            );
            it(std:("should format rocket messages"), [=]() mutable
            {
                expect(emoji["rocket"](std:("Launch!")))->toBe(std:("🚀 Launch!"));
            }
            );
            it(std:("should format package messages"), [=]() mutable
            {
                expect(emoji["package"](std:("Package!")))->toBe(std:("📦 Package!"));
            }
            );
            it(std:("should format link messages"), [=]() mutable
            {
                expect(emoji["link"](std:("Link!")))->toBe(std:("🔗 Link!"));
            }
            );
            it(std:("should format tip messages"), [=]() mutable
            {
                expect(emoji["tip"](std:("Tip!")))->toBe(std:("💡 Tip!"));
            }
            );
            it(std:("should format list messages"), [=]() mutable
            {
                expect(emoji["list"](std:("List!")))->toBe(std:("📋 List!"));
            }
            );
            it(std:("should format penguin messages"), [=]() mutable
            {
                expect(emoji["penguin"](std:("Linux!")))->toBe(std:("🐧 Linux!"));
            }
            );
            it(std:("should format bullet messages"), [=]() mutable
            {
                expect(emoji["bullet"](std:("Item")))->toBe(std:("• Item"));
            }
            );
        }
        );
        describe(std:("initializeEmojiSupport"), [=]() mutable
        {
            it(std:("should log emoji support status in debug mode"), [=]() mutable
            {
                process->env->DEBUG = std:("true");
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), std:("darwin")}, 
                    object::pair{std:("configurable"), true}
                });
                process->env->TERM = std:("xterm-256color");
                initializeEmojiSupport();
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), originalPlatform}, 
                    object::pair{std:("configurable"), true}
                });
            }
            );
            it(std:("should not log when not in debug mode"), [=]() mutable
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
        describe(std:("platform-specific emoji support"), [=]() mutable
        {
            it(std:("should support emojis on Linux with proper terminal"), [=]() mutable
            {
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), std:("linux")}, 
                    object::pair{std:("configurable"), true}
                });
                process->env->TERM = std:("xterm-256color");
                expect(getEmoji(std:("success")))->toBe(std:("✅"));
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), originalPlatform}, 
                    object::pair{std:("configurable"), true}
                });
            }
            );
            it(std:("should support emojis with COLORTERM set"), [=]() mutable
            {
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), std:("linux")}, 
                    object::pair{std:("configurable"), true}
                });
                process->env->COLORTERM = std:("truecolor");
                expect(getEmoji(std:("success")))->toBe(std:("✅"));
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), originalPlatform}, 
                    object::pair{std:("configurable"), true}
                });
            }
            );
            it(std:("should detect Windows Terminal support"), [=]() mutable
            {
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), std:("win32")}, 
                    object::pair{std:("configurable"), true}
                });
                process->env->WT_SESSION = std:("some-session-id");
                expect(getEmoji(std:("success")))->toBe(std:("✅"));
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), originalPlatform}, 
                    object::pair{std:("configurable"), true}
                });
            }
            );
            it(std:("should detect PowerShell 7+ support"), [=]() mutable
            {
                auto originalPlatform = process->platform;
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), std:("win32")}, 
                    object::pair{std:("configurable"), true}
                });
                process->env->PSModulePath = std:("C:\Program Files\PowerShell\7\Modules");
                process->env->POWERSHELL_TELEMETRY_OPTOUT = std:("1");
                expect(getEmoji(std:("success")))->toBe(std:("✅"));
                Object->defineProperty(process, std:("platform"), object{
                    object::pair{std:("value"), originalPlatform}, 
                    object::pair{std:("configurable"), true}
                });
            }
            );
            it(std:("should not support emojis in GitHub Actions"), [=]() mutable
            {
                process->env->GITHUB_ACTIONS = std:("true");
                expect(getEmoji(std:("success")))->toBe(std:("[OK]"));
            }
            );
        }
        );
    }
    );
}

MAIN
