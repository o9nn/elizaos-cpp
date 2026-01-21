#include "emoji-handler.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool detectEmojiSupport() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Force disable if requested
    if (config.forceDisable) {
        return false;
    }

    // Check environment variables that indicate emoji support
    const auto term = process.env.TERM || "";
    const auto termProgram = process.env.TERM_PROGRAM || "";
    const auto colorTerm = process.env.COLORTERM;
    const auto ciEnv = process.env.CI;

    // CI environments often don't render emojis well
    if (ciEnv == 'true' || process.env.GITHUB_ACTIONS) {
        return false;
    }

    // Windows Command Prompt and older terminals
    if (process.platform == 'win32') {
        // Windows Terminal, VS Code terminal, and newer terminals support emojis
        if (
        termProgram == "vscode" ||;
        process.env.WT_SESSION ||;
        process.env.WT_PROFILE_ID ||;
        termProgram == "Windows Terminal";
        ) {
            return true;
        }

        // PowerShell 7+ generally supports emojis
        if (process.env.PSModulePath && process.env.POWERSHELL_TELEMETRY_OPTOUT != undefined) {
            return true;
        }

        // Default to false for Windows unless we're sure
        return false;
    }

    // macOS and Linux
    if (process.platform == 'darwin' || process.platform == 'linux') {
        // Modern terminals support emojis
        if (
        termProgram == "vscode" ||;
        termProgram == "iTerm.app" ||;
        termProgram == "Apple_Terminal" ||;
        (std::find(term.begin(), term.end(), "xterm") != term.end()) ||;
        (std::find(term.begin(), term.end(), "screen") != term.end()) ||;
        colorTerm;
        ) {
            return true;
        }
    }

    // Check for specific terminal capabilities
    if (term.includes('256color') || term.includes('truecolor')) {
        return true;
    }

    // Conservative default - assume no emoji support unless we're confident
    return false;

}

std::string getEmoji(EmojiKey key) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto emojiDef = EMOJIS[key];
    if (!emojiDef) {
        std::cout << "Unknown emoji key: " + key << std::endl;
        return "";
    }

    return config.enabled && detectEmojiSupport() ? emojiDef.emoji : emojiDef.fallback;

}

void configureEmojis(const std::optional<EmojiConfig>& newConfig) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    config = { ...config, ...newConfig }

}

EmojiConfig getEmojiConfig() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return { ...config }

}

bool areEmojisEnabled() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return config.enabled && !config.forceDisable && detectEmojiSupport();

}

std::string withEmoji(EmojiKey key, const std::string& message, bool spacing = true) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto emoji = getEmoji(key);
    const auto space = spacing && emoji ? " " : "";
    return emoji + space + message;

}

void initializeEmojiSupport() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto supported = detectEmojiSupport();

    // Log emoji support status in debug mode
    if (process.env.DEBUG || process.env.ELIZA_DEBUG) {
        logger.debug(
        "Emoji support: " + std::to_string(supported ? "enabled" : "disabled") + " (platform: " + process.platform + ", term: " + std::to_string(process.env.TERM || "unknown") + ")"
        );
    }

}

} // namespace elizaos
